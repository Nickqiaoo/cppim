#include "RpcServer.h"
#include "log.h"

void *p;
int main() {
    common::Log::Instance().Init("tcpserver","./log","trace","debug",true, 1);
    string ip = "127.0.0.1";
    RpcServer server(5, ip, 8080);
    server.start();
    while (1) {
        sleep(100);
    }
    return 0;
}