#pragma once

#include <memory>
#include <functional>
#include "log.h"
#include "librdkafka/rdkafkacpp.h"

typedef std::function<void(RdKafka::Message*, void*)> KafkaMessageCallback;

class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {


    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        if (event.fatal()) {
            LOG_ERROR("FATAL");
        }
        LOG_ERROR("{} : {}",event.err(),event.str());
        break;

      case RdKafka::Event::EVENT_STATS:
        LOG_ERROR("{}",event.str());
        break;

      case RdKafka::Event::EVENT_LOG:
        LOG_ERROR("{}-{}: {}",event.severity(),event.fac(),event.str());
        break;

      case RdKafka::Event::EVENT_THROTTLE:
        LOG_ERROR("THROTTLED: {} ms by {} id {}",event.throttle_time(),event.broker_name(),event.broker_id());
	break;

      default:
        LOG_ERROR("EVENT {} ({})",event.type(),RdKafka::err2str(event.err()),event.str());
        break;
    }
  }
};


class ExampleRebalanceCb : public RdKafka::RebalanceCb {
private:
  static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions){
    for (unsigned int i = 0 ; i < partitions.size() ; i++)
        LOG_ERROR("{} [{}]",partitions[i]->topic(),partitions[i]->partition());
  }

public:
  void rebalance_cb (RdKafka::KafkaConsumer *consumer,
		     RdKafka::ErrorCode err,
                     std::vector<RdKafka::TopicPartition*> &partitions) {
    LOG_ERROR("RebalanceCb: {}", RdKafka::err2str(err));

    part_list_print(partitions);

    if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
      consumer->assign(partitions);
    } else {
      consumer->unassign();
    }
  }
};

class KafkaConsumer{
    public:
    KafkaConsumer(const std::string& brokers, const std::string& topic);
    ~KafkaConsumer();
    
    void SubTopic(const std::string& topic){
        topic_.push_back(topic);
        consumer_->subscribe(topic_);
    }

    void Start();
    void setMseeageCallback(const KafkaMessageCallback& cb){
        messagecb_ = cb;
    }

    private:
    KafkaMessageCallback messagecb_;
    std::vector<std::string> topic_;
    std::string brokers_;
    std::shared_ptr<RdKafka::KafkaConsumer> consumer_;
    std::shared_ptr<RdKafka::Conf> conf_;
    std::shared_ptr<RdKafka::Conf> tconf_;
};