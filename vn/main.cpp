#include "Router.h"
#include "Controller.h"
#include "UI.h"
#include "Helper.h"
#include <iostream>
#include <fstream>
using namespace std;

string local_ip;
int port;


void runInRouter() {
    Router router(local_ip, ROUTER_PORT);
    RouterUI::Operating(router);
}

void runInController() {
    Controller center_controller(local_ip, CONTROLLER_PORT);
    center_controller.UpdateInterval();
    center_controller.Receive();
    while(1); 
}

//读取本机IP
void getLocalIP() {
	cout << endl << "Getting Local Configuration，Please wait..." << endl << endl; 
	ifstream fin;
	fin.open(".\\neighbors.txt");
	string temp;
	getline(fin, temp); //第一行为本机IP。
	local_ip = temp;
	return;
} 

int main(int argc, char** argv) {
    string type;
    cout << "Router or Controller?  ( 0 -> Router | other -> Controller )" << endl;
    cin >> type;
	
	getLocalIP();
	
    if (type == "0") { // Router 
        runInRouter();
    } else {     // Controller 
        runInController();
    }
    return 0;
}


