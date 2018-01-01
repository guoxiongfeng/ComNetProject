#ifndef ROUTER_H
#define ROUTER_H
#include <Winsock2.h>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct Datagram {
	Datagram() {
	}
	
	Datagram(string s, string sip, string dip) {
		msg = s;
		src_ip = sip;
		dst_ip = dip;
	}
	
	string msg; 
	string src_ip;
	string dst_ip;
	int dst_port;
};


class Route {
	public:
		string dst_ip;
		int cost;
		
		Route(string d, int c) {
			dst_ip = d;
			cost = c;
		}
};

class LocalRoute {
	public:
		string dst_ip;
		string next_hop;
		int cost;
		
		
		LocalRoute(string d, string n, int c) {
			dst_ip = d;
			next_hop = n;
			cost = c;
		}
};

class Router {
	private:
		int port;
		
		string local_ip;
		
		vector< pair<string, int> > neighbours;
		
		vector<LocalRoute> local_table;
		
		map<string, bool> is_On;
		
		map<string, bool> neighbor_up;
		
		void Get_Neightbors();

		void Save_Neighbors();
		
		static void * Listen(void * data);
		
		static void * Regular_Broadcast(void * data);
		
		void Route_Table_Calculate(vector<Route> routes, string src);
		
		void Inform_Neighbors();
		
		void Respond_Inform(string dst_ip);
		
		void Deliver_Message(Datagram & data);
		
		int Get_Neighbor_Cost(string neighbor_ip);
		
		LocalRoute Get_Local_Route(string dst_ip);
		
		int Get_Local_Table_Index(string ip);
		
	public:
		Router(string local_ip, int port);

		string Get_Local_IP() { return local_ip; }
		
		void Communication(string msg, string dst);

		void Send(string ip, int port, Datagram & data);
		
		pthread_t Receive();
		
		void Keep_Alive();
		
		void Print_Routes();

		bool Modify_Route(string dst_ip, string next_hop, int cost);

		bool Add_Route(string dst_ip, string next_hop, int cost);

		bool Delete_Route(string dst_ip);

		void setRoutingProtocol(int tag);
		
};

void setTrigger(bool b);

#endif
