#include <iostream>
#include <cstdlib>
#include <string>
#include <signal.h> 
#include "UI.h"

using namespace std;

void RouterUI::Operating(Router& router) {
	RouterUI::RouterManageUI();

	do {
		string oper;
		cin >> oper;
		if (oper == "1") {
				RouterUI::ShowRouterTable(router);
				cout << "press any key to go back..." << endl;
				system("pause");
				RouterUI::RouterManageUI();
		} else if (oper == "2") {
				RouterUI::ChangeRouterTable(router);
				cout << "press any key to go back..." << endl;
				system("pause");
				RouterUI::RouterManageUI();
		} else if (oper == "3") {
				RouterUI::ChangeRoutingProtocol(router);
				cout << "press any key to go back..." << endl;
				system("pause");
				RouterUI::RouterManageUI();
		} else if (oper == "4") {
				SendAndRecvUI::SendingAndReceiving(router);
				cout << "press any key to go back..." << endl;
				system("pause");
				RouterUI::RouterManageUI();
		} else if (oper == "5") {
				cout << "thanks for using, bye." << endl;
				break;
		} else {
				cout << "instruction error! please enter again: " << endl;
		}
	} while (true);
}

void RouterUI::RouterManageUI() {
	system("cls");
	cout << "\t\t+-----------------------------------------+" << endl;
	cout << "\t\t|                                         |" << endl;
	cout << "\t\t|         Router Table Management         |" << endl;
	cout << "\t\t|                                         |" << endl;
	cout << "\t\t+-----------------------------------------+" << endl;
	cout << endl;
	cout << "\t\t   1. show router table" << endl;
	cout << "\t\t   2. change router table" << endl;
	cout << "\t\t   3. change routing protocol" << endl;
	cout << "\t\t   4. send message" << endl;
	cout << "\t\t   5. exit" << endl;
	cout << endl;
	cout << "\t\t   Please enter your choice: ";
}

void RouterUI::ShowRouterTable(Router& router) {
	cout << "router table in this host: " << endl;
	cout << endl;

	router.Print_Routes();

	cout << endl;
}

void RouterUI::ChangeRouterTable(Router& router) {
	system("cls");
	RouterUI::ShowRouterTable(router);
	cout << "Operations:" << endl;
	cout << endl;
	cout << "1. modify route information" << endl;
	cout << "2. add route information" << endl;
	cout << "3. delete route information" << endl;
	cout << "4. exit" << endl;
	cout << endl;

	do {
		cout << "Please enter your choice: ";
		int oper;
		cin >> oper;
		while (oper < 1 || oper > 4) {
			cout << "instruction error! please enter again: ";
			cin >> oper;
		}

		string dstIP, nextHop;
		int cost;
		switch (oper) {
			case 1:
				cout << "Enter destination IP address route to modify: " << endl;
				cin >> dstIP;
				cout << "Enter new infomation of next hop or cost of this route: " << endl;
				cin >> nextHop >> cost;
				if (router.Modify_Route(dstIP, nextHop, cost)) { cout << "success!" << endl; RouterUI::ShowRouterTable(router); }
				else { cout << "fail to modify the route, please check your dstIP and try again." << endl; }
				break;
			case 2:
				cout << "Enter destination IP address, next hop and cost of the new route: " << endl;
				cin >> dstIP >> nextHop >> cost;
				if (router.Add_Route(dstIP, nextHop, cost)) { cout << "success!" << endl; RouterUI::ShowRouterTable(router); }
				else { cout << "fail to add the route, please check your information and try again." << endl; }
				break;
			case 3:
				cout << "Enter destination IP address of the route to delete: " << endl;
				cin >> dstIP;
				if (router.Delete_Route(dstIP)) { cout << "success!" << endl; RouterUI::ShowRouterTable(router); }
				else { cout << "fail to delete the route, please check your dstIP and try again." << endl; }
				break;
			case 4:
				return ;
		}
	} while (true);
}

void RouterUI::ChangeRoutingProtocol(Router& router) {
	system("cls");
	cout << "Enter \"ls\" or \"dv\" to change routing protocol to LS or DV:" << endl;
	string str;
	while (1) {
		cin >> str;
		if (str == "ls" || str == "LS") {
			setTrigger(false);
			break;
		} else if (str == "dv" || str == "DV") {
			setTrigger(true);
			break;
		} else {
			cout << "Invalid instruction. please enter again." << endl;
		}
	}
}

void SendAndRecvUI::SendingAndReceiving(Router& router) {
	pthread_t rcv_thread = router.Receive();
	router.Keep_Alive();
	while (true) {
		//system("cls");
		string msg, dst_ip;
		cout << "Please enter message and destination IP address: " << endl;
		cin >> msg;
		if (msg == "exit") {
			//pthread_kill(rcv_thread, 3);
			break;
		}
		cin >> dst_ip;
		router.Communication(msg, dst_ip);
	}
}
