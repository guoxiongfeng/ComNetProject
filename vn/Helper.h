#ifndef HELPPER_HPP
#define HELPPER_HPP
#include "Router.h"
#include <vector>

#define DOWN 0
#define LS 1
#define DV 2
#define CONTROLLER_PORT 47318
#define ROUTER_PORT 12345


void Socket_init(int port, SOCKET & socket1, sockaddr_in & local);

//��һ���ַ���ת��Ϊ������ 
int ToNum(string &s);

//��һ��������ת��Ϊ�ַ�����ʽ�� 
string stringfy(int k);

//serializeһ��datagram 
string stringfy(Datagram & datagram);

//serializeһ��Route
string stringfy(Route & route);

string stringfy(LocalRoute & rt);

//ͬPython�� split�� ��֧���ַ��ָ 
vector<string> split(string s, char x);

vector<Route> ToRouteItems(string s);

//��һ�����л��ַ���deserialize�� ת����datagram��ʽ�� 
Datagram ToDatagram(string s);

//yanglikun
Route ToRoute(string s);

string stringfy(vector<LocalRoute> & vector_rt);
vector<LocalRoute> ToLocalRouteItems(string s);

void Debug(string s);

#endif
