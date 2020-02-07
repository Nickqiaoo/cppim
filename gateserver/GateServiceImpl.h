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
    virtual void Broadcast(::google::protobuf::RpcController* controller, const ::gate::BroadcastReq* request, ::gate::BroadcastReply* reply,
                           ::google::protobuf::Closure* done) override;
    virtual void BroadcastRoom(::google::protobuf::RpcController* controller, const ::gate::BroadcastRoomReq * request, ::gate::BroadcastRoomReply* reply,
                           ::google::protobuf::Closure* done) override;
   private:
    GateServer* gateserver_;
};