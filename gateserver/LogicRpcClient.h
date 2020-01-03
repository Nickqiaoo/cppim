#pragma once

#include "Loop.h"
#include "RpcChannel.h"
#include "logic.pb.h"
#include "log.h"

#include <stdio.h>
#include <unistd.h>
#include <functional>

class GateServer;
using namespace std::placeholders;

class LogicRpcClient {
   public:
    LogicRpcClient(LoopPtr loop) : session_(std::make_shared<Session>(loop, 1)), channel_(new RpcChannel), stub_(channel_.get()) {
        session_->setConnectionCallback(std::bind(&LogicRpcClient::onConnection, this, _1));
        session_->setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2));
        // client_.enableRetry();
    }

    void connect(const std::string& ip, int port) { session_->connect(ip,port); }

    void Connect(logic::ConnectReq* request, SessionPtr session) {
        logic::ConnectReply* response = new logic::ConnectReply;
        stub_.Connect(NULL, request, response, NewCallback(gateserver_, &GateServer::HandleConnect, response, session));
    }
   private:
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        channel_->setConnection(conn);
    }

    private:
    GateServer* gateserver_;
    SessionPtr session_;
    RpcChannelPtr channel_;
    logic::Logic_Stub stub_;
};