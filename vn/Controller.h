#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "Router.h"
#include <map>
#include <vector>

#define MAX_TTL 100

class Controller{
	public :
		Controller(string local_ip, int port);
		
		//请求LS状态， 各个主机把自己的LS状态接收到后给Controller。用Route的结构。 
		void RequestState();
		
		//根据不同目标主机， 将计算得到的route table发送过去。 
		void SendLocalRoute(string dst_ip, vector<LocalRoute> & rt_table);
		
		//Listen 和 Receive 考虑了一下，还是写一个和Router 不同的。
		void Receive();
		//定时执行。
		void UpdateInterval(); 
		
	private :

		
		string local_ip;
		int port;
		//edges. 
		vector<vector<int> > edges;
		map<string, int> index;
		vector<string> nodes;

		//从外存读取网络中IP信息，并进行处理（IP用数字标识之类） 
		void ControllerInit();
		vector<LocalRoute> dijkstra_i(int i);
		static void * Update(void * data);
		static void *ListenClient(void * data);
		void InitEdges(); 
		void Send(string ip, int port, Datagram & data);
}; 

#endif
