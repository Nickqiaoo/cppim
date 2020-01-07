#pragma once

#include <string>

#include "ClientCodec.h"
#include "GateServiceImpl.h"
#include "LogicRpcClient.h"
#include "RpcServer.h"

class GateServer {
   public:
    GateServer(LoopPtr loop, int thrnum, const std::string& tcpip, int tcpport, const std::string& rpcip, int rpcport);
    ~GateServer();

    void HandleConnect(logic::ConnectReply* response, const SessionPtr session);
    void Start();
    void SendToClient(const std::string& key, const gate::Proto& msg);
    void SendToRoom(const std::string& roomid, const std::string& body);
    void BroadCast(const std::string& body);
   private:
    void onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body);

   private:
    TcpServer tcpserver_;
    RpcServer rpcserver_;

    LogicRpcClient rpcclient_;
    GateServiceImpl gateservice_;

    ClientCodec clientcodec_;

    std::mutex mutex_;
    std::string serverid_;
    std::unordered_map<std::string, SessionPtr> channels_; // key->session
    std::unordered_map<std::string, std::unordered_set<SessionPtr>> rooms_;
};