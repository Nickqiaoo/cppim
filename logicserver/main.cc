#include "LogicServer.h"
#include "log.h"

int main() {
    common::Log::Instance().Init("logicserver","../log/logicserver.log","trace","debug",true, 1);
    std::string ip = "127.0.0.1";
    auto loop = std::make_shared<Loop>();
    LogicServer server(loop,3, ip, 8080,ip,8081);
    server.Start();
    loop->start();
    while (1) {
        sleep(100);
    }
    return 0;
}