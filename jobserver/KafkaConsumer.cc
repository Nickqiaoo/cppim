#include "KafkaConsumer.h"

KafkaConsumer::KafkaConsumer(const std::string& brokers)
    :brokers_(brokers), conf_(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL)), tconf_(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC)) {
    std::string errstr;
    conf_->set("metadata.broker.list", brokers_, errstr);
    conf_->set("group.id", "test", errstr);
    conf_->set("default_topic_conf", tconf_.get(), errstr);
    consumer_ = std::shared_ptr<RdKafka::KafkaConsumer>(RdKafka::KafkaConsumer::create(conf_.get(), errstr));
}

KafkaConsumer::~KafkaConsumer(){
    consumer_->close();
}