#pragma once
#include <map>
#include "Acceptor.h"
#include "LoopMgr.h"
#include "net_define.h"

class TcpServer {
  TcpServer();
  ~TcpServer();

 private:
  std::map<int, SessionPtr> connections_;
  LoopMgr loopmgr_;
  Acceptor acceptor_;
};