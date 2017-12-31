#include "Router.h"
#include "Controller.h"
#include "UI.h"
#include <iostream>
using namespace std;

string ip;
int port;


void runInRouter() {
    Router router(ip, port);
    RouterUI::Operating(router);
}

void runInController() {
    Controller center_controller(ip, port);
    center_controller.UpdateInterval();
    center_controller.Receive();
    while(1); 
}

int main(int argc, char** argv) {
    string type;
    cout << "Roter or Controller?  ( 0 -> Router | other -> Controller )" << endl;
    cin >> type;
    cout << "Ip and port?  ( 0.0.0.0 8080 )" << endl;
    cin >> ip >> port;

    if (type == "0") { // Router 
        runInRouter();
    } else {     // Controller 
        runInController();
    }
    return 0;
}


