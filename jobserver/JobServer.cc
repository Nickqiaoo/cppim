#include "JobServer.h"
#include "log.h"
#include "logic.pb.h"

void JobServer::HandleKafkaMessage(RdKafka::Message* message, void* opaque) {
    // LOG_ERROR("message err: {}", message->err());
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR: {
            /* Real message */
            LOG_INFO("Read msg at offset {}", message->offset());
            RdKafka::MessageTimestamp ts;
            ts = message->timestamp();
            if (ts.type != RdKafka::MessageTimestamp::MSG_TIMESTAMP_NOT_AVAILABLE) {
                std::string tsname = "?";
                if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_CREATE_TIME)
                    tsname = "create time";
                else if (ts.type == RdKafka::MessageTimestamp::MSG_TIMESTAMP_LOG_APPEND_TIME)
                    tsname = "log append time";
                LOG_INFO("Timestamp: {} {}", tsname, ts.timestamp);
            }
            if (message->key()) {
                LOG_INFO("Key: {}", *message->key());
            }
            LOG_INFO("message len: {}", message->len());
            logic::PushMsg msg;
            if (msg.ParseFromArray(message->payload(), message->len())) {
                auto pushmsg = new gate::PushMsgReq();
                gate::PushMsgReply* response = new gate::PushMsgReply;
                pushmsg->mutable_keys()->CopyFrom(msg.keys());
                pushmsg->set_protoop(msg.operation());
                auto proto = pushmsg->mutable_proto();
                proto->set_ver(1);
                proto->set_op(9);
                proto->set_body(msg.msg());

                rpcclient_.PushMsg(pushmsg, response, NewCallback(this, &JobServer::HandlePushMsg, response));
            } else {
                LOG_ERROR("parse from kafka error");
            }
        } break;

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
            break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            LOG_ERROR("Consume failed: {}", message->errstr());
            // run = false;
            break;

        default:
            /* Errors */
            LOG_ERROR("Consume failed: {}", message->errstr());
            // run = false;
    }
}

void JobServer::HandlePushMsg(gate::PushMsgReply* response) { LOG_INFO("HandlePushMsg"); }