#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "Router.h"
#include <map>
#include <vector>

#define MAX_TTL 100

class Controller{
	public :
		Controller(string local_ip, int port);
		
		//����LS״̬�� �����������Լ���LS״̬���յ����Controller����Route�Ľṹ�� 
		void RequestState();
		
		//���ݲ�ͬĿ�������� ������õ���route table���͹�ȥ�� 
		void SendLocalRoute(string dst_ip, vector<LocalRoute> & rt_table);
		
		//Listen �� Receive ������һ�£�����дһ����Router ��ͬ�ġ�
		void Receive();
		//��ʱִ�С�
		void UpdateInterval(); 
		
	private :

		
		string local_ip;
		int port;
		//edges. 
		vector<vector<int> > edges;
		map<string, int> index;
		vector<string> nodes;

		//������ȡ������IP��Ϣ�������д���IP�����ֱ�ʶ֮�ࣩ 
		void ControllerInit();
		vector<LocalRoute> dijkstra_i(int i);
		static void * Update(void * data);
		static void *ListenClient(void * data);
		void InitEdges(); 
		void Send(string ip, int port, Datagram & data);
}; 

#endif
