#include "JobServer.h"
#include "log.h"

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
                push(msg);
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

void JobServer::push(const logic::PushMsg& msg) {
    switch (msg.type()) {
        case logic::PushMsg::PUSH:
            pushKeys(msg.operation(), msg.server(), msg.keys(), msg.msg());
            break;
        case logic::PushMsg::ROOM:
            broadcastRoom(msg.room(), msg.msg());
            break;
        case logic::PushMsg::BROADCAST:
            broadcast(msg.operation(), msg.msg(), msg.speed());
            break;
        default:
            break;
    }
}
void JobServer::pushKeys(int32_t operation, const string& server, const google::protobuf::RepeatedPtrField<std::string>& keys,
                         const std::string& msg) {
    auto pushmsg = new gate::PushMsgReq();
    gate::PushMsgReply* response = new gate::PushMsgReply;
    pushmsg->mutable_keys()->CopyFrom(keys);
    pushmsg->set_protoop(operation);
    auto proto = pushmsg->mutable_proto();
    proto->set_ver(1);
    proto->set_op(9);
    proto->set_body(msg);
    rpcclient_.PushMsg(pushmsg, response, NewCallback(this, &JobServer::HandlePushMsg, response));
}

void JobServer::broadcastRoom(const std::string& room, const std::string& msg) {
    auto pushmsg = new gate::BroadcastRoomReq();
    auto response = new gate::BroadcastRoomReply();
    pushmsg->set_roomid(room);
    auto proto = pushmsg->mutable_proto();
    proto->set_ver(1);
    proto->set_op(9);
    proto->set_body(msg);
    rpcclient_.BroadcastRoom(pushmsg, response, nullptr);
}

void JobServer::broadcast(int32_t operation, const std::string& msg, int32_t speed) {
    auto pushmsg = new gate::BroadcastReq();
    auto response = new gate::BroadcastReply();
    pushmsg->set_speed(speed);
    pushmsg->set_protoop(operation);
    auto proto = pushmsg->mutable_proto();
    proto->set_ver(1);
    proto->set_op(9);
    proto->set_body(msg);
    rpcclient_.Broadcast(pushmsg, response, nullptr);
}

void JobServer::HandlePushMsg(gate::PushMsgReply* response) { LOG_INFO("HandlePushMsg"); }