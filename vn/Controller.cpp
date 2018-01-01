#include "Controller.h"
#include <fstream> 
#include <iostream>
#include "Helper.h"
#include <Winsock2.h>

 
Controller::Controller(string local_ip, int port) {
	this->local_ip = local_ip;
	this->port = port;
	this->ControllerInit();
}

void Controller::ControllerInit() {
	Debug(string("Initializing Controller, Please wait..."));
	ifstream fin;
	fin.open(".\\route_nodes.txt");
	string temp;
	while (getline(fin, temp)) {
		index[temp] = nodes.size();
		nodes.push_back(temp);
	}
	fin.close();
	InitEdges();
	Debug(string("Initializing Successfully."));
}

void Controller::RequestState() {
	for (int i = 0; i < nodes.size(); ++i) {
		Datagram d("10", local_ip, nodes[i]);
		//��ֱ���� ֱ�ӷ���ȥ�� 
		Send(nodes[i], ROUTER_PORT, d);
		//router �ӵ�Controller �ı��ģ� ֻ����������� һ����Request�� һ����Response�� 
		//�ֱ��ӦController������ͷ��ؽ���� 
	}
}

void Controller::UpdateInterval() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Update, (void *)this);
} 

void * Controller::Update(void * data) {
	Controller *instance = (Controller *)data;
	while (1) {
		Sleep(4000);
		//���֮ǰ��edges��Ϣ�� 
		instance->InitEdges();
		instance->RequestState();
		//�ȴ�1s��Ӧʱ�䡣�绹û�н��յ��� �Ͳ��ܡ�������ʱ��Ϊ������ 
		Sleep(2000); 
		for (int i = 0; i < instance->nodes.size(); ++i) {
			vector<LocalRoute> ith_node_info = instance->dijkstra_i(i);
			instance->SendLocalRoute(instance->nodes[i], ith_node_info);
		} 
	}
}
	
void Controller::SendLocalRoute(string dst_ip, vector<LocalRoute> & rt_table) {
	//Tostring ��Route�ṹ�� ת��Ϊstring �����л��� 
	string rt_info = string("11") + stringfy(rt_table);
	//std::cout << dst_ip << "   DIJKSTRA:" << rt_info << std::endl << std::endl << std::endl;
	Datagram d(rt_info, local_ip, dst_ip);
	Send(dst_ip, ROUTER_PORT, d); 
} 

void Controller::Receive() {
	pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, ListenClient, (void *)this);
}

void * Controller::ListenClient(void * data) { 
	Controller * instance = (Controller *)data;
	int port = instance->port;
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
		//����Ŀ�ĵصı���ֱ�����ˣ�������
		if (datagram.dst_ip == instance->local_ip && datagram.msg.substr(0, 2) == "10") {
			//debug��Ϣ 
			//cout <<"[receive] "<< "msg:"<< datagram.msg << " src:" << datagram.src_ip << " dst:" << datagram.dst_ip << " dst_port:" << datagram.dst_port << endl;
			//ֻ����������client��·����Ϣ�� 
			//ToRoute����msg �����л�Ϊ Route �ṹ�� 
			vector<Route> r_info = ToRouteItems(datagram.msg.substr(2)); 
			//���±ߡ�
			//send һ��Route��Ŀ�ͷ�һ�����ıȽϺý����� 
			for (int i = 0; i < r_info.size(); ++i) {
				if (instance->index.count(r_info[i].dst_ip) == 0) {
					instance->index[r_info[i].dst_ip] = instance->nodes.size();
					instance->nodes.push_back(r_info[i].dst_ip);
					vector<int> tmp(instance->nodes.size(), MAX_TTL); // add row
					//add column
					for (int j = 0; j < instance->edges.size(); ++j) {
						instance->edges[j].push_back(MAX_TTL);
					}
					instance->edges.push_back(tmp);
					instance->edges[instance->edges.size() - 1][instance->edges.size() - 1] = 0;
				}
				cout << datagram.src_ip << "   " << r_info[i].dst_ip << "   " << r_info[i].cost << endl;
				//if (r_info[i].dst_ip == instance->local_ip) continue; //�������controller��ͨ�ıߡ� 
				
				instance->edges[instance->index[datagram.src_ip]][instance->index[r_info[i].dst_ip]] = r_info[i].cost;
				//instance->edges[instance->index[r_info[i].dst_ip]][instance->index[datagram.src_ip]] = r_info[i].cost;
			}
			cout << endl << endl << endl;
		}
	}
}

void Controller::InitEdges() {
	edges.clear();
	edges.resize(nodes.size());
	for (int i = 0; i < nodes.size(); ++i) {
		edges[i].resize(nodes.size());
		for (int j = 0; j < nodes.size(); ++j) {
			if (j == i) edges[i][j] = 0;
			else edges[i][j] = MAX_TTL + 1;//��ʾ���ɴ� 
		}
	}
}

void Controller::Send(string dst_ip, int dst_port, Datagram & data) {
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
    server.sin_port = htons(dst_port);
    server.sin_addr.s_addr = inet_addr(dst_ip.c_str());
    socket1 = socket(AF_INET, SOCK_DGRAM, 0);
    string buffer = stringfy(data);
    if (sendto(socket1, buffer.c_str(), strlen(buffer.c_str()), 0, (struct sockaddr*)&server, len) != SOCKET_ERROR) { 
		//Debug(string("Send to " + dst_ip + " success."));
	} 
}

void Deal_Down(vector<vector<int> > & edges) {
	for (int i = 0; i < edges.size(); ++i) {
		for (int j = 0; j < edges.size(); ++j) {
			if (edges[i][j] >= MAX_TTL || edges[j][i] >= MAX_TTL) {
				edges[i][j] = MAX_TTL;
				edges[j][i] = MAX_TTL;
			}
		}
	}
}

vector<LocalRoute> Controller::dijkstra_i(int src) {
	vector<int> next_hop(nodes.size(), -1);
	vector<bool> visit(nodes.size());
	vector<int> D(nodes.size());
	
	Deal_Down(edges);
	
	//��ʼ���� 
	for (int i = 0; i < edges.size(); ++i) {
		D[i] = edges[src][i];
	}
	visit[src] = true; next_hop[src] = src;
	
	for (int j = 0; j < nodes.size() - 1; ++j) {
		int min_index = -1;
		for (int i = 0; i < nodes.size(); ++i) {
			if (!visit[i] &&  (min_index == -1 || D[i] < D[min_index])) {
				min_index = i;
			}
		}
		//cout << min_index << endl;
			//��srcֱ��Ϊ��̵���� 
		if (D[min_index] == edges[src][min_index]) {
			next_hop[min_index] = min_index; 
		}

		visit[min_index] = true;
		for (int i = 0; i < nodes.size(); ++i) {
			if (!visit[i] && D[min_index] + edges[min_index][i] < D[i]) {
				D[i] = D[min_index] + edges[min_index][i];
				next_hop[i] = next_hop[min_index];
			}
		}
		
	}
	//change next_hop to LocalRoute.
	vector<LocalRoute> rt_table;
	for (int i = 0; i < nodes.size(); ++i) {
		rt_table.push_back(LocalRoute(nodes[i], nodes[next_hop[i]], D[i]));
	}
	return rt_table;
	
}

/*
//Center ���֡�
//��ʱ�������� �����Ϣ�� dijkstra�� �ַ���Ϣ��
*/
