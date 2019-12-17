#pragma once
#include <map>
#include "Acceptor.h"
#include "LoopMgr.h"
#include "net_define.h"
#include "Session.h"

class TcpServer {
  public:
  TcpServer(int thrnum, const std::string& ip, int port);
  ~TcpServer();

  SessionPtr newSession();
  void start();
 private:
  std::map<int, SessionPtr> connections_;
  uint64_t sessionid_{0};
  LoopMgr loopmgr_;
  Acceptor acceptor_;
};