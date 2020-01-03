#pragma once

#include "rdkafkacpp.h"
#include <memory>

class KafkaProducer {
   public:
    KafkaProducer(const std::string& broker);
    ~KafkaProducer();
    void Produce(const std::string& topic,const std::string& key, const std::string& value);

   private:
   std::string broker_;
   std::shared_ptr<RdKafka::Producer> producer_;
   std::shared_ptr<RdKafka::Conf> conf_;
};