#pragma once

#include <memory>
#include "librdkafka/rdkafkacpp.h"
#include "log.h"
#include "net_define.h"

class ProduceCb : public RdKafka::DeliveryReportCb {
   public:
    void dr_cb(RdKafka::Message& message) {
        /* If message.err() is non-zero the message delivery failed permanently
         * for the message. */
        if (message.err())
            LOG_ERROR("Message delivery failed: {}", message.errstr());
        else
            LOG_INFO("Message delivery topic: {} [{}] at offset {}", message.topic_name(), message.partition(), message.offset());\
        Callback* cb = static_cast<Callback*>(message.msg_opaque());
        if(cb!=nullptr){
            (*cb)();
            delete cb;
        }
    }
};

class KafkaProducer {
   public:
    KafkaProducer(const std::string& broker, const std::string& topic);
    ~KafkaProducer();
    
    void Produce(const std::string& key, const std::string& value, Callback* cb);
    void Poll(){
        while(true){
            producer_->poll(-1);
        }
    }

   private:
    std::string broker_;
    std::string topic_;
    ProduceCb cb_;
    std::shared_ptr<RdKafka::Producer> producer_;
    std::shared_ptr<RdKafka::Conf> conf_;
};