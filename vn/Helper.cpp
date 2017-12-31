#include "Router.h"
#include "Controller.h"
#include <vector>
#include <iostream>

void Socket_init(int port, SOCKET & socket1, sockaddr_in & local) {
	local.sin_family = AF_INET;
    local.sin_port = htons(port);  //the port need listened
    local.sin_addr.s_addr = INADDR_ANY; //localhost
    socket1 = socket(AF_INET,SOCK_DGRAM, 0);  //UDP 
}

//°ÑÒ»¸ö×Ö·û´®×ª»¯ÎªÕýÕûÊý 
int ToNum(string &s) {
	int ans = 0;
	for (int i = 0; i < s.size(); ++i) {
		ans = ans * 10 + s[i] - '0';
	}
	return ans;
}

//°ÑÒ»¸öÕýÕûÊý×ª»¯Îª×Ö·û´®ÐÎÊ½¡£ 
string stringfy(int k) {
	if (k == 0) return "0";
	string s;
	while (k) {
		s.insert(s.begin(), k % 10 + '0');
		k/= 10;
	}
	return s;
}


//Í¬PythonÖÐ split£¬ ½öÖ§³Ö×Ö·û·Ö¸î¡£ 
vector<string> split(string s, char x) {
	vector<string> ans;
	int index = 0;
	for (int i = 0; i < s.size(); ++i) {
		if (s[i] == x) {
			ans.push_back(s.substr(index, i - index));
			index = i + 1;
		}
	}
	return ans;
}


//serializeÒ»¸ödatagram 
string stringfy(Datagram & datagram) {
	string s = datagram.msg + "#" + datagram.src_ip + "#" + datagram.dst_ip + "#";
	return s;
}

//serializeÒ»¸öRoute
string stringfy(Route & route) {
	string s = route.dst_ip + "$" + stringfy(route.cost) + "$";
	return s;
}


string stringfy(LocalRoute & rt) {
	string s = rt.dst_ip + "$" + rt.next_hop + "$" + stringfy(rt.cost) + "$";
	return s;
}

string stringfy(vector<LocalRoute> & vector_rt) {
	string ans = "";
	for (int i = 0; i < vector_rt.size(); ++i) {
		if (vector_rt[i].cost < MAX_TTL) {
			ans += stringfy(vector_rt[i]);
		}
	}
	return ans;
}
vector<LocalRoute> ToLocalRouteItems(string s) {
	vector<string> tmp = split(s, '$');
	vector<LocalRoute> local_rt;
	for (int i = 0; i < tmp.size(); i += 3) {
		LocalRoute local_r(tmp[i], tmp[i + 1], ToNum(tmp[i + 2]));
		local_rt.push_back(local_r);
	}
	return local_rt;
}




vector<Route> ToRouteItems(string s) {
	vector<string> tmp = split(s, '$');
	vector<Route> rt;
	for (int i = 0; i < tmp.size(); i += 2) {
		Route r(tmp[i], ToNum(tmp[i + 1]));
		rt.push_back(r);
	}
	return rt;
}

//°ÑÒ»¸öÐòÁÐ»¯×Ö·û´®deserialize£¬ ×ª»¯»Ødatagram¸ñÊ½¡£ 
Datagram ToDatagram(string s) {
	vector<string> re = split(s, '#');
	Datagram d(re[0], re[1], re[2]);
	return d;
}

//yanglikun
Route ToRoute(string s) {
	vector<string> v = split(s, '$');
	return Route(v[0], ToNum(v[1]));
}

void Debug(string s) {
	cout << endl << s << endl;
}


