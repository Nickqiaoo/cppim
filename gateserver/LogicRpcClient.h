#pragma once;

#include "Loop.h"
#include "RpcChannel.h"
#include "Tcpserver.h"
#include "logic.pb.h"
#include "log.h"

#include <stdio.h>
#include <unistd.h>
#include <functional>

using namespace std::placeholders;

class LogicRpcClient {
   public:
    LogicRpcClient(LoopPtr loop) : session_(std::make_shared<Session>(loop, 1)), channel_(new RpcChannel), stub_(channel_.get()) {
        session_->setConnectionCallback(std::bind(&LogicRpcClient::onConnection, this, _1));
        session_->setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2));
        // client_.enableRetry();
    }

    void connect() { session_->connect("127.0.0.1", 8080); }

   private:
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        channel_->setConnection(conn);
    }

    void Connect(logic::ConnectReq* request) {
        logic::ConnectReply* response = new logic::ConnectReply;
        stub_.Connect(NULL, request, response, NewCallback(this, &LogicRpcClient::HandleConnect, response));
    }
    private:
    void HandleConnect(logic::ConnectReply* response);
    SessionPtr session_;
    RpcChannelPtr channel_;
    logic::Logic_Stub stub_;
};