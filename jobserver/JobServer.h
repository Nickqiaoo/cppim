#pragma once
#include "GateRpcClient.h"
#include "KafkaConsumer.h"

class JobServer {
   public:
    JobServer(LoopPtr loop) : rpcclient_(loop) {}
    ~JobServer();

   private:
    GateRpcClient rpcclient_;
    KafkaConsumer kafkaconsumer_;
};