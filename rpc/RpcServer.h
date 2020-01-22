#pragma once

#include "TcpServer.h"

namespace google {
namespace protobuf {

class Service;

}  // namespace protobuf
}  // namespace google


class RpcServer
{
 public:
  RpcServer(int thrnum, const std::string& ip, int port);

  //void setThreadNum(int numThreads)
  //{
  //  server_.setThreadNum(numThreads);
  //}

  void registerService(::google::protobuf::Service*);
  void start();
  void stop();

 private:
  void onConnection(const SessionPtr& conn);

  // void onMessage(const TcpConnectionPtr& conn,
  //                Buffer* buf,
  //                Timestamp time);

  TcpServer server_;
  std::map<std::string, ::google::protobuf::Service*> services_;
};

