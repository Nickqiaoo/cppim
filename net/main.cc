#include "RpcServer.h"
#include "log.h"
#include "echo.pb.h"

class EchoServiceImpl : public echo::EchoService
{
 public:
  virtual void Echo(::google::protobuf::RpcController* controller,
                       const ::echo::EchoRequest* request,
                       ::echo::EchoResponse* response,
                       ::google::protobuf::Closure* done)
  {
    response->set_msg("hello");
    LOG_INFO("receive msg:{}",request->msg());
    done->Run();
  }
};

int main() {
    common::Log::Instance().Init("tcpserver","./log","trace","debug",true, 1);
    string ip = "127.0.0.1";
    RpcServer server(5, ip, 8080);
    EchoServiceImpl impl;
    server.start();
    server.registerService(&impl);
    while (1) {
        sleep(100);
    }
    return 0;
}