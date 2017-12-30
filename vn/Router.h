#ifndef ROUTER_H
#define ROUTER_H
#include <Winsock2.h>
#include <string>
#include <vector>
#include <stdlib.h>

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
		
		void Get_Neightbors();

		void Save_Neighbors();
		
		void Boardcast(Datagram & data);
		
		static void * Listen(void * data);
		
		static void * Regular_Broadcast(void * data);
		
		void Route_Table_Calculate(vector<Route> routes, string src);
		
		void Inform_Neighbors();
		
		//string choosePath(string &dst_ip);
		
	public:
		Router(string local_ip, int port);

		string Get_Local_IP() { return local_ip; }

		void Send(string ip, Datagram & data);
		
		void Receive();
		
		void Keep_Alive();
		
		void Print_Routes();

		bool Modify_Route(string dst_ip, string next_hop, int cost);

		bool Add_Route(string dst_ip, string next_hop, int cost);

		bool Delete_Route(string dst_ip);

		void setRoutingProtocol(int tag);
		
	//yanglikun
};

void setTrigger(bool b);

#endif
