#include "LogicServer.h"
#include "log.h"

int main() {
    common::Log::Instance().Init("logicserver","../log/logicserver.log","trace","debug",false, 1);
    std::string ip = "127.0.0.1";
    LogicServer server(3, ip, 8082,ip,8083);
    server.Start();
    while (1) {
        sleep(100);
    }
    return 0;
}