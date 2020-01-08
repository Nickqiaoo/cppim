#include "JobServer.h"
#include "log.h"

int main() {
    common::Log::Instance().Init("jobserver","../log/jobserver.log","trace","debug",true, 1);
    std::string ip = "127.0.0.1";
    auto loop = std::make_shared<Loop>();
    JobServer server(loop,"localhost:9092");
    server.Start();
    loop->start();
    server.StartConsum(); 
    return 0;
}