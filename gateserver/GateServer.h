#pragma once

#include <string>
#include "LogicRpcClient.h"
#include "GateServiceImpl.h"
#include "ClientCodec.h"
#include "RpcServer.h"
#include "TcpServer.h"

class GateServer {
   public:
    GateServer();
    ~GateServer();

   private:
   void onClientMessageCallback();
   private:
    TcpServer tcpserver_;
    RpcServer rpcserver_;
    LogicRpcClient rpcclient_;
    GateServiceImpl gateservice_;
    ClientCodec clientcodec_;

    std::mutex mutex_;
    std::string serverid_;
    std::unordered_map<std::string, SessionPtr> channels_;
    std::unordered_map<std::string, std::unordered_set<SessionPtr>> rooms_;
};