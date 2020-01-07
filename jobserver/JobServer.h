#pragma once
#include "GateRpcClient.h"
#include "KafkaConsumer.h"

class JobServer {
   public:
    JobServer(LoopPtr loop, const std::string& brokers) : rpcclient_(loop) ,kafkaconsumer_(brokers){}
    ~JobServer();

    void Start(){
        kafkaconsumer_.Start();
    }

    void HandleKafkaMessage(RdKafka::Message* message, void* opaque);
    void HandlePushMsg(gate::PushMsgReply* response);

   private:
    GateRpcClient rpcclient_;
    KafkaConsumer kafkaconsumer_;
};