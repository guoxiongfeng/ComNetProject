#include <Winsock2.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "Helper.h"
#include "Router.h"

using namespace std;

void * Router::Listen(void * data) {
	Router * this_router = (Router *)data;
	int port = ((Router *)data)->port;
	SOCKET socket_rec;
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 1), &wsaData);// windows socket dll;
	struct sockaddr_in local;
	struct sockaddr_in client;
	Socket_init(port, socket_rec, local);
	bind(socket_rec, (struct sockaddr*)&local, sizeof(local));
	int len = sizeof(client);
	char buffer[1024] = "\0";
	while (recvfrom(socket_rec, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &len) != SOCKET_ERROR) {
		Datagram datagram = ToDatagram(string(buffer));
		//is dest? output : route.
		if (datagram.msg[0] != '1' && datagram.src_ip == ((Router*)data)->local_ip) {
			continue;
		} else if (datagram.dst_ip != this_router->local_ip) { 
			this_router->Deliver_Message(datagram);
			Debug(string("Transmit packet to " + datagram.dst_ip + "..."));
		}
		else {
			Debug(string(datagram.msg + "   " + datagram.src_ip + "   " + datagram.dst_ip));
			if (datagram.msg.substr(0, 2) == "00") {
				vector<Route> routes = ToRouteItems(datagram.msg.substr(2));
				this_router->Route_Table_Calculate(routes, datagram.src_ip);
				this_router->Respond_Inform(datagram.src_ip);
			} else if (datagram.msg.substr(0, 2) == "01") {
				Debug(string("Receive message: " + datagram.msg.substr(2)));
			} else if (datagram.msg.substr(0, 2) == "10") {                             // Â§ÑÁêÜÊù•Ëá™center‰∏ªÊú∫ÁöÑ‰ø°ÊÅØ
				string route_info = "10";
				for (int i = 0; i < this_router->neighbours.size(); ++i) {
					Route r(this_router->neighbours[i].first, this_router->neighbours[i].second);
					route_info += stringfy(r);
				}
				Datagram d(route_info, this_router->local_ip, datagram.src_ip);
				this_router->Send(datagram.src_ip, CONTROLLER_PORT, d);
			} else if (datagram.msg.substr(0, 2) == "11") {  
				this_router->local_table = ToLocalRouteItems(datagram.msg.substr(2));
				this_router->Print_Routes();
			} else if (datagram.msg.substr(0, 2) == "02") {
				this_router->is_On[datagram.src_ip] = true;
				if (this_router->local_table[this_router->Get_Local_Table_Index(datagram.src_ip)].cost > this_router->Get_Neighbor_Cost(datagram.src_ip)) {
					this_router->local_table[this_router->Get_Local_Table_Index(datagram.src_ip)].cost = this_router->Get_Neighbor_Cost(datagram.src_ip);
					this_router->local_table[this_router->Get_Local_Table_Index(datagram.src_ip)].next_hop = datagram.src_ip;
				} 
			}
		}
	}
}

void Router::Respond_Inform(string dst_ip) {
	string msg = "02";
	Datagram data(msg, this->local_ip, dst_ip);
	Send(dst_ip, this->port, data);
}

bool trigger = true;
void setTrigger(bool b) {
	trigger = b;
}

void * Router::Regular_Broadcast(void * data) {
	Router * this_router = (Router *)data;
	while (1) {
		if (trigger) {
			this_router->Inform_Neighbors();
			for (int i = 0; i < this_router->neighbours.size(); i++) {
				this_router->is_On[this_router->neighbours[i].first] = false;
			}
			Sleep(1000);
			for (int i = 0; i < this_router->neighbours.size(); i++) {
				this_router->neighbor_up[this_router->neighbours[i].first] = this_router->is_On[this_router->neighbours[i].first];
				if (this_router->is_On[this_router->neighbours[i].first] == false) {
					//cout << "Disconnected: " << this_router->neighbours[i].first << endl;
					for (int j = 0; j < this_router->local_table.size(); j++) {
						if (this_router->local_table[j].next_hop == this_router->neighbours[i].first) {
							this_router->local_table[j].cost = INT_MAX;
						}
					}
					this_router->local_table[this_router->Get_Local_Table_Index(this_router->neighbours[i].first)].cost = INT_MAX;
				}
			}
			Sleep(7000);
			this_router->Print_Routes();
			/*
			if ((double)(b - a) / CLOCKS_PER_SEC > 10) {
				this_router->Inform_Neighbors();
				a = clock();
				this_router->Print_Routes();
			}
			for (int i = 0; i < this_router->neighbours.size(); i++) {
				if (this_router->respond_time[this_router->neighbours[i].first] < 0) {
					continue;
				}
				if ((double)(b - this_router->respond_time[this_router->neighbours[i].first]) / CLOCKS_PER_SEC > 3) {
					this_router->local_table[this_router->Get_Local_Table_Index(this_router->neighbours[i].first)].cost = INT_MAX;
					this_router->neighbor_up[this_router->neighbours[i].first] = false;
				}
			}
			*/
		}
	}
	
}

void Router::Keep_Alive() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Regular_Broadcast, (void *)this);
}

int Router::Get_Neighbor_Cost(string neighbor_ip) {
	int c = INT_MAX;
	for (int k = 0; k < neighbours.size(); k++) {
		if (neighbours[k].first == neighbor_ip) {
			c = neighbours[k].second;
			return c;
		}
	}
	return c;
}



int Router::Get_Local_Table_Index(string ip) {
	int c = -1;
	for (int k = 0; k < local_table.size(); k++) {
		if (local_table[k].dst_ip == ip) {
			c = k;
			return c;
		}
	}
	return c;
}

LocalRoute Router::Get_Local_Route(string dst_ip) {
	for (int i = 0; i < local_table.size(); ++i) {
		if (dst_ip == local_table[i].dst_ip) {
			return local_table[i];
		}
	}
	return LocalRoute("", "", 0);
}


void Router::Route_Table_Calculate(vector<Route> routes, string src) {
	LocalRoute local_to_mid = Get_Local_Route(src);
	if (local_to_mid.cost == INT_MAX || local_to_mid.dst_ip == "") {
		return;
	}
	for (int i = 0; i < routes.size(); i++) {
		if (routes[i].dst_ip == this->local_ip) {
			continue;
		}
		int j;
		for (j = 0; j < local_table.size(); j++) {
			if (local_table[j].dst_ip == routes[i].dst_ip) {
				if (local_table[j].next_hop == local_to_mid.next_hop) {
					if (routes[i].cost == INT_MAX) {
						local_table[j].cost = INT_MAX;
					} else if (Get_Neighbor_Cost(routes[i].dst_ip) == INT_MAX || Get_Neighbor_Cost(routes[i].dst_ip) > routes[i].cost + local_to_mid.cost  && neighbor_up[routes[i].dst_ip]){
						local_table[j].cost = routes[i].cost + local_to_mid.cost;
					}
				} else if (routes[i].cost != INT_MAX && local_table[j].cost > routes[i].cost + local_to_mid.cost) {
					if (Get_Neighbor_Cost(routes[i].dst_ip) == INT_MAX || Get_Neighbor_Cost(routes[i].dst_ip) > routes[i].cost + local_to_mid.cost || !neighbor_up[routes[i].dst_ip]) {
						local_table[j].next_hop = local_to_mid.next_hop;
						local_table[j].cost = routes[i].cost + local_to_mid.cost;
					}
					else {
						local_table[j].next_hop = routes[i].dst_ip;
						local_table[j].cost = Get_Neighbor_Cost(routes[i].dst_ip);
					}
				}
				break; 
			}
		}
		if (routes[i].cost != INT_MAX && j == local_table.size()) {
			local_table.push_back(LocalRoute(routes[i].dst_ip, src, routes[i].cost + local_to_mid.cost));
		}
	}
}

void Router::Inform_Neighbors() {
	for (int i = 0; i < this->neighbours.size(); i++) {
		Datagram send_rip = Datagram();
		send_rip.msg = "00";
		send_rip.src_ip = this->local_ip;
		send_rip.dst_ip = neighbours[i].first;
		for (int j = 0; j < local_table.size(); j++) {
			Route rt = Route(local_table[j].dst_ip, local_table[j].cost);
			send_rip.msg = send_rip.msg + stringfy(rt);
		}
		this->Send(neighbours[i].first, this->port, send_rip);
	}
}

void Router::Communication(string msg, string dst) {
	string proto = "01";
	Datagram data(proto + msg, this->local_ip, dst);
	Deliver_Message(data);
} 

void Router::Deliver_Message(Datagram & datagram) {
	string next_hop = "";
	int i;
	for (i = 0; i < local_table.size(); ++i) {
		if (datagram.dst_ip == local_table[i].dst_ip) {
			next_hop = local_table[i].next_hop;
			break;
		}
	}
	if (i == local_table.size()) {
		Debug(string("Can not find path."));
		return ; 
	}
	Send(next_hop, this->port, datagram);
}
//Œ™¡À«¯∑÷≤ªÕ¨∑¢ÀÕ∂Àø⁄°£ 
void Router::Send(string dst_ip, int aim_port, Datagram & data) {
	SOCKET socket1;
    WSADATA wsaData;
    int ErrorCode;
    if (WSAStartup(MAKEWORD(2,1), &wsaData)) {
    	Debug(string("Winsock initiate failed!"));
        WSACleanup();
        return;
    }

    struct sockaddr_in server;
    int len = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(aim_port);
    server.sin_addr.s_addr = inet_addr(dst_ip.c_str());
    socket1 = socket(AF_INET, SOCK_DGRAM, 0);
    string buffer = stringfy(data);
    if (sendto(socket1, buffer.c_str(), strlen(buffer.c_str()), 0, (struct sockaddr*)&server, len) != SOCKET_ERROR) { 
		//Debug(string("Send to " + dst_ip + " success."));
	} 
}

pthread_t Router::Receive() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Listen, (void *)this);
    return tid;
}

Router::Router(string ip, int port) {
	this->local_ip = ip;
	this->port = port;
	Get_Neightbors();
}

void Router::Get_Neightbors(){
	ifstream fin;
	fin.open(".\\neighbors.txt");
	string temp;
	getline(fin, temp); //Ã¯π˝µ⁄“ª––£¨ µ⁄“ª––Œ™±æª˙IP°£ 
	while (getline(fin, temp)) {
		vector<string> tmp = split(temp, ' ');
		if (temp.size() != 0) {
			neighbours.push_back(pair<string, int>(tmp[0], ToNum(tmp[1])));
			local_table.push_back(LocalRoute(tmp[0], tmp[0], ToNum(tmp[1])));
			neighbor_up[tmp[0]] = false;
			local_table[Get_Local_Table_Index(tmp[0])].cost = INT_MAX;
		}		
	}
	fin.close();
}

void Router::Save_Neighbors() {
	ofstream fout;
	fout.open("neighbors.txt");

	for (int i = 0; i < neighbours.size() - 1; ++i) {
		fout << neighbours[i].first << ' ' << neighbours[i].second << ' ' << endl;
	}
	fout << neighbours[neighbours.size() - 1].first << ' ' << neighbours[neighbours.size() - 1].second << ' ';

	fout.close();
}

bool Router::Modify_Route(string dst_ip, string next_hop, int cost) {
	for (int i = 0; i < neighbours.size(); ++i) {
		if (neighbours[i].first == dst_ip) {
			neighbours[i].second = cost;
			local_table[i].next_hop = next_hop;
			local_table[i].cost = cost;

			Save_Neighbors();
			return true;
		}
	}
	Debug(string("Cannot find route whose destinaton IP address is " + dst_ip + "!"));
	return false;
}

bool Router::Add_Route(string dst_ip, string next_hop, int cost) {
	for (int i = 0; i < neighbours.size(); ++i) {
		if (neighbours[i].first == dst_ip) {
			Debug(string("destinaton IP address " + dst_ip + " has been existed in the route table!"));
			return false;
		}
	}
	neighbours.push_back(pair<string, int>(dst_ip, cost));
	local_table.push_back(LocalRoute(dst_ip, next_hop, cost));
	Save_Neighbors();

	return true;
}

bool Router::Delete_Route(string dst_ip) {
	vector< pair<string, int> >::iterator it1;
	vector<LocalRoute>::iterator it2;

	for (it1 = neighbours.begin(), it2 = local_table.begin(); it1 != neighbours.end(); ++it1, ++it2) {
		if (it1 -> first == dst_ip) {
			neighbours.erase(it1);
			local_table.erase(it2);
			Save_Neighbors();
			return true;
		}
	}
	Debug(string("Cannot find route whose destinaton IP address is " + dst_ip + "!"));
	return false;
}

void Router::Print_Routes() {
	for (int i = 0; i < local_table.size(); i++) {
		Debug(string("Dst IP: " + local_table[i].dst_ip + " / Next Hop: " +
		local_table[i].next_hop + " / Cost: " + stringfy(local_table[i].cost)));
	}
	cout << endl;
}


