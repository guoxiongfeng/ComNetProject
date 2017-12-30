#ifndef HELPPER_HPP
#define HELPPER_HPP
#include "Router.h"
#include <vector>

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

#endif