#include "GateServer.h"
#include "log.h"

int main() {
    common::Log::Instance().Init("gateserver","../log/gateserver.log","trace","debug",true, 1);
    string ip = "127.0.0.1";
    auto loop = std::make_shared<Loop>();
    GateServer server(loop,3, ip, 8080,ip,8081);
    server.Start();
    loop->start();
    while (1) {
        sleep(100);
    }
    return 0;
}