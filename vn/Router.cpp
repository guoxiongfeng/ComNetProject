
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
		if (datagram.dst_ip == ((Router*)data)->local_ip) {
			cout <<  datagram.msg << "   " << datagram.src_ip << "   " << datagram.dst_ip << endl;
		} else if (datagram.src_ip == ((Router*)data)->local_ip) {
			continue;
		} else {
			//temp deal : boardcast.
			if (datagram.msg.substr(0, 2) == "00") {
				vector<Route> routes = ToRouteItems(datagram.msg.substr(2));
				this_router->Route_Table_Calculate(routes, datagram.src_ip);
				this_router->Inform_Neighbors();
			} else if (datagram.msg.substr(0, 2) == "01") {
				cout << "Transmit packet to " << datagram.dst_ip << "..." << endl;
				this_router->Boardcast(datagram);
			} else if (datagram.msg.substr(0, 2) == "10") {                             // Â§ÑÁêÜÊù•Ëá™center‰∏ªÊú∫ÁöÑ‰ø°ÊÅØ
				//yanglikun
				for (int i = 0; i < this_router->neighbours.size(); ++i) {
					Route r(this_router->neighbours[i].first, this_router->neighbours[i].second);
					Datagram d(stringfy(r), this_router->local_ip, datagram.src_ip);
					this_router->Send(datagram.src_ip, d);
				}
			}
		}
	}
}

bool trigger = true;
void setTrigger(bool b) {
	trigger = b;
}

void * Router::Regular_Broadcast(void * data) {
	Router * this_router = (Router *)data;
	clock_t a = clock();
	while (1) {
	//yanglikun
		if (trigger) {
			clock_t b = clock();
			if ((double)(b - a)/ CLOCKS_PER_SEC > 10) {
				this_router->Inform_Neighbors();
				a = clock();
				this_router->Print_Routes();
			}
		}
	}
	
}

void Router::Keep_Alive() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Regular_Broadcast, (void *)this);
}

void Router::Route_Table_Calculate(vector<Route> routes, string src) {
	for (int i = 0; i < local_table.size(); i++) {
		for (int j = 0; j < routes.size(); j++) {
			if (local_table[i].dst_ip == routes[j].dst_ip) {
				if (local_table[i].cost > routes[j].cost) {
					local_table[i].next_hop = src;
					local_table[i].cost = routes[j].cost;
				}
				break; 
			}
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
		this->Send(neighbours[i].first, send_rip);
	}
}

void Router::Send(string dst_ip, Datagram & data) {
	string next_hop = "0.0.0.0";
	for (int i = 0; i < local_table.size(); ++i) {
		if (dst_ip == local_table[i].dst_ip) {
			next_hop = local_table[i].next_hop;
			break;
		}
	}
	if (next_hop == "0.0.0.0") {
		//√ª”–œ‡πÿ¬∑”…–≈œ¢
		cout << "Can not find path."<< endl;
		return ; 
	}
	
	SOCKET socket1;
    WSADATA wsaData;
    int ErrorCode;
    if (WSAStartup(MAKEWORD(2,1), &wsaData)) {
    	cout << "Winsock initiate failed!" << endl;
        WSACleanup();
        return;
    }

    struct sockaddr_in server;
    int len = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(this->port);
    server.sin_addr.s_addr = inet_addr(dst_ip.c_str());
    socket1 = socket(AF_INET, SOCK_DGRAM, 0);
    string buffer = stringfy(data);
    if (sendto(socket1, buffer.c_str(), strlen(buffer.c_str()), 0, (struct sockaddr*)&server, len) != SOCKET_ERROR) { 
		cout << "Send to " << next_hop << " success." << endl;
	} 
}

void Router::Receive() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Listen, (void *)this);
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
	while (getline(fin, temp)) {
		vector<string> tmp = split(temp, ' ');
		if (temp.size() != 0) {
			neighbours.push_back(pair<string, int>(tmp[0], ToNum(tmp[1])));
			//local_table.push_back(LocalRoute(tmp[0], this->local_ip, ToNum(tmp[1])));
			local_table.push_back(LocalRoute(tmp[0], tmp[0], ToNum(tmp[1])));
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

	cout << "cannot find route whose destinaton IP address is " << dst_ip << "!" << endl;
	return false;
}

bool Router::Add_Route(string dst_ip, string next_hop, int cost) {
	for (int i = 0; i < neighbours.size(); ++i) {
		if (neighbours[i].first == dst_ip) {
			cout << "destinaton IP address " << dst_ip << " has been existed in the route table!" << endl;
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

	cout << "cannot find route whose destinaton IP address is " << dst_ip << "!" << endl;
	return false;
}

void Router::Print_Routes() {
	for (int i = 0; i < local_table.size(); i++) {
		cout << "Dst IP: " << local_table[i].dst_ip << " / Next Hop: " << local_table[i].next_hop << " / Cost: " << local_table[i].cost << endl;
	}
	cout << endl;
}

void Router::Boardcast(Datagram & data) {
	for(int i = 0; i < neighbours.size(); ++i) {
		if (neighbours[i].first == local_ip)  {
			continue;
		}
		cout << "Boardcast to " << neighbours[i].first << endl;
		Send(neighbours[i].first, data);
	}
}


