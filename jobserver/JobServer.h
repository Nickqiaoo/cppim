#pragma once
#include "GateRpcClient.h"
#include "KafkaConsumer.h"

class JobServer {
   public:
    JobServer(LoopPtr loop, const std::string& brokers) : rpcclient_(this, loop), kafkaconsumer_(brokers) {
        kafkaconsumer_.setMseeageCallback(std::bind(&JobServer::HandleKafkaMessage, this, _1, _2));
    }
    ~JobServer(){};

    void Start() {
        rpcclient_.connect("127.0.0.1", 8081);
    }

    void StartConsum(){
        kafkaconsumer_.Start();
    }

    void HandleKafkaMessage(RdKafka::Message* message, void* opaque);
    void HandlePushMsg(gate::PushMsgReply* response);

   private:
    GateRpcClient rpcclient_;
    KafkaConsumer kafkaconsumer_;
};