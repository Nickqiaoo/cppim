#pragma once
#include <map>
#include "Acceptor.h"
#include "LoopMgr.h"
#include "Session.h"
#include "net_define.h"

class TcpServer {
 public:
  TcpServer(int thrnum, const std::string& ip, int port);
  ~TcpServer();

  SessionPtr newSession();
  void start();
  void setMessageCallback(const onMessageCallback& cb) {
    messagecallback_ = cb;
  }

 private:
  std::map<int, SessionPtr> connections_;
  uint64_t sessionid_{0};
  LoopMgr loopmgr_;
  Acceptor acceptor_;
  onMessageCallback messagecallback_;
};