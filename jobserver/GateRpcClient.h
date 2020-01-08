#pragma once

#include "Loop.h"
#include "RpcChannel.h"
#include "gate.pb.h"
#include "log.h"

#include <stdio.h>
#include <unistd.h>
#include <functional>

class JobServer;
using namespace std::placeholders;

class GateRpcClient {
   public:
    GateRpcClient(JobServer* server, LoopPtr loop) :jobserver_(server), session_(std::make_shared<Session>(loop, 1)), channel_(new RpcChannel), stub_(channel_.get()) {
        session_->setConnectionCallback(std::bind(&GateRpcClient::onConnection, this, _1));
        session_->setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2));
        // client_.enableRetry();
    }

    void connect(const std::string& ip, int port) { session_->connect(ip,port); }

    void PushMsg(gate::PushMsgReq* request); 
   private:
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        channel_->setConnection(conn);
    }

    private:
    JobServer* jobserver_;
    SessionPtr session_;
    RpcChannelPtr channel_;
    gate::Gate_Stub stub_;
};