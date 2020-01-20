#include "LogicServiceImpl.h"
#include <chrono>
#include "LogicServer.h"
#include "nlohmann/json.hpp"

void LogicServiceImpl::Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                               ::google::protobuf::Closure* done) {
    LOG_INFO("client connect:{}", request->token());
    try {
        auto jsonbody = nlohmann::json::parse(request->token());
        auto it = jsonbody.find("mid");
        int mid;
        if (it != jsonbody.end() && it->is_number()) {
            mid = it.value();
            response->set_mid(mid);
        }
        response->set_roomid(jsonbody["room_id"]);
        std::string key = logicserver_->generateKey();
        response->set_key(key);
        std::vector<int> accepts = jsonbody["accepts"];
        for (auto it : accepts) {
            response->add_accepts(it);
        }
        auto serverid = jsonbody["server"];
        addServerMap(jsonbody["mid"], key, request->server());
    } catch (std::exception& e) {
        LOG_TRACE("parse json error");
    }
    done->Run();
}

void LogicServiceImpl::Heartbeat(::google::protobuf::RpcController* controller, const ::logic::HeartbeatReq* request,
                                 ::logic::HeartbeatReply* response, ::google::protobuf::Closure* done) {
    LOG_INFO("client heartbeat server:{} mid:{} key:{}", request->server(),request->mid(), request->key());
    
}

std::string LogicServiceImpl::keyMidServer(int32_t mid) { return fmt::format("mid_{}", mid); }

std::string LogicServiceImpl::keyKeyServer(const std::string& key) { return fmt::format("key_{}", key); }

void LogicServiceImpl::addServerMap(const int32_t mid, const std::string& key, const std::string& server) {
    std::vector<std::string> commandvec;
    commandvec.emplace_back(fmt::format("HSET {} {} {}", keyMidServer(mid), key, server));
    commandvec.emplace_back(fmt::format("EXPIRE {} {}", keyMidServer(mid), 30 * 60));
    commandvec.emplace_back(fmt::format("SET {} {}", keyKeyServer(key), server));
    commandvec.emplace_back(fmt::format("EXPIRE {} {}", keyKeyServer(key), 30 * 60));
    redisclient_.PipeLine(commandvec);
}
