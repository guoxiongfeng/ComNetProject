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
    while(1); 
}

int main(int argc, char** argv) {
    int type;
    cout << "Roter or Controller?  ( 0 -> Router | 1 -> Controller )" << endl;
    cin >> type;
    cout << "Ip and port?  ( 0.0.0.0 8080 )" << endl;
    cin >> ip >> port;

    if (!type) { // Router 
        runInRouter();
    } else {     // Controller 
        runInController();
    }
    return 0;
}


