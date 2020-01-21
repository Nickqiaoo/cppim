#pragma once

#include "RedisClient.h"
#include "log.h"
#include "logic.pb.h"

class LogicServer;

class LogicServiceImpl : public logic::Logic {
   public:
    LogicServiceImpl(LogicServer* server, const std::string& redisip, const int redisport) : logicserver_(server), redisclient_(redisip, redisport) {
        if (!redisclient_.Connect()) {
            LOG_ERROR("redis connect error");
        }
    }

   public:
    virtual void Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                         ::google::protobuf::Closure* done) override;
    virtual void Heartbeat(::google::protobuf::RpcController* controller, const ::logic::HeartbeatReq* request, ::logic::HeartbeatReply* response,
                           ::google::protobuf::Closure* done) override;
    virtual void Disconnect(::google::protobuf::RpcController* controller, const ::logic::DisconnectReq* request, ::logic::DisconnectReply* response,
                           ::google::protobuf::Closure* done) override;

    std::string keyMidServer(int32_t mid);
    std::string keyKeyServer(const std::string& key);

   private:
    void addServerMap(const int32_t mid, const std::string& key, const std::string& server);

   private:
    LogicServer* logicserver_;
    RedisClient redisclient_;
};