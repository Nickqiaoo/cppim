#pragma once

#include "librdkafka/rdkafkacpp.h"
#include <memory>

class ProduceCb : public RdKafka::DeliveryReportCb {
public:
  void dr_cb (RdKafka::Message &message) {
    /* If message.err() is non-zero the message delivery failed permanently
     * for the message. */
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
    else
      std::cerr << "% Message delivered to topic " << message.topic_name() <<
        " [" << message.partition() << "] at offset " <<
        message.offset() << std::endl;
  }
};

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