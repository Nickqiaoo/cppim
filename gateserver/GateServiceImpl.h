#pragma once

#include "gate.pb.h"
#include "log.h"

class GateServer;

class GateServiceImpl : public gate::Gate {
   public:
    GateServiceImpl(GateServer* server) : gateserver_(server) {}

   public:
    virtual void PushMsg(::google::protobuf::RpcController* controller, const ::gate::PushMsgReq* request, ::gate::PushMsgReply* response,
                         ::google::protobuf::Closure* done) override;

   private:
    GateServer* gateserver_;
};