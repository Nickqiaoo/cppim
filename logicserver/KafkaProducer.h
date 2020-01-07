#pragma once

#include <memory>
#include "librdkafka/rdkafkacpp.h"
#include "log.h"

class ProduceCb : public RdKafka::DeliveryReportCb {
   public:
    void dr_cb(RdKafka::Message& message) {
        /* If message.err() is non-zero the message delivery failed permanently
         * for the message. */
        if (message.err())
            LOG_ERROR("Message delivery failed: {}", message.errstr());
        else
            LOG_INFO("Message delivery topic: {} [{}] at offset {}", message.topic_name(), message.partition(), message.offset());
    }
};

class KafkaProducer {
   public:
    KafkaProducer(const std::string& broker);
    ~KafkaProducer();
    
    void Produce(const std::string& topic, const std::string& key, const std::string& value);

   private:
    std::string broker_;
    std::shared_ptr<RdKafka::Producer> producer_;
    std::shared_ptr<RdKafka::Conf> conf_;
};