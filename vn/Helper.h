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

//把一个字符串转化为正整数 
int ToNum(string &s);

//把一个正整数转化为字符串形式。 
string stringfy(int k);

//serialize一个datagram 
string stringfy(Datagram & datagram);

//serialize一个Route
string stringfy(Route & route);

string stringfy(LocalRoute & rt);

//同Python中 split， 仅支持字符分割。 
vector<string> split(string s, char x);

vector<Route> ToRouteItems(string s);

//把一个序列化字符串deserialize， 转化回datagram格式。 
Datagram ToDatagram(string s);

//yanglikun
Route ToRoute(string s);

string stringfy(vector<LocalRoute> & vector_rt);
vector<LocalRoute> ToLocalRouteItems(string s);

void Debug(string s);

#endif
