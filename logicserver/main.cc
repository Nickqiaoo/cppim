#include "LogicServer.h"
#include "log.h"

int main() {
    common::Log::Instance().Init("logicserver","../log/logicserver.log","trace","debug",true, 1);
    std::string ip = "127.0.0.1";
    LogicServer server(3, ip, 8080,ip,8081);
    server.Start();
    while (1) {
        sleep(100);
    }
    return 0;
}