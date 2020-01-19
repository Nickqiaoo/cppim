#pragma once
#include "GateRpcClient.h"
#include "KafkaConsumer.h"
#include "logic.pb.h"

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
    private:
    void push(const logic::PushMsg& msg);
    void pushRoom();
    void pushKeys(int32_t operation, const string& server, const google::protobuf::RepeatedPtrField<std::string>& keys, const std::string& msg);
    void HandlePushMsg(gate::PushMsgReply* response);
   private:
    GateRpcClient rpcclient_;
    KafkaConsumer kafkaconsumer_;
};