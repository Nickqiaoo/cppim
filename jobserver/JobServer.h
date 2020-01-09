#pragma once
#include "GateRpcClient.h"
#include "KafkaConsumer.h"

class JobServer {
   public:
    JobServer(LoopPtr loop, const std::string& brokers, const std::string& topic, const std::string& clientip, int clientport)
        : rpcclient_(clientip, clientport, this, loop), kafkaconsumer_(brokers, topic) {
        kafkaconsumer_.setMseeageCallback(std::bind(&JobServer::HandleKafkaMessage, this, _1, _2));
    }
    ~JobServer(){};

    void Start() { rpcclient_.connect(); }

    void StartConsum() { kafkaconsumer_.Start(); }

    void HandleKafkaMessage(RdKafka::Message* message, void* opaque);
    void HandlePushMsg(gate::PushMsgReply* response);

   private:
    GateRpcClient rpcclient_;
    KafkaConsumer kafkaconsumer_;
};