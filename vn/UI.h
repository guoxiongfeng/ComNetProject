#ifndef UI_H
#define UI_H

#include "Router.h"

class RouterUI {
public:
	static void Operating(Router& router);
	// operations for router tabel
	static void RouterManageUI();

	static void ShowRouterTable(Router& router);

	static void ChangeRouterTable(Router& router);

	static void ChangeRoutingProtocol(Router& router);
};

class SendAndRecvUI {
public:
	static void SendingAndReceiving(Router& router);
};

#endif
