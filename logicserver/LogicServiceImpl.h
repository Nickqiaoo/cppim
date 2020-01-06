#pragma once

#include "log.h"
#include "logic.pb.h"
#include "KafkaProducer.h"

class LogicServer;

class LogicServiceImpl : public logic::Logic {
   public:
    virtual void Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                         ::google::protobuf::Closure* done) override;

   private:
    LogicServer* logicserver_;
    KafkaProducer kafkaproducer_;
};