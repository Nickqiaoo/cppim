#include "LogicServiceImpl.h"
#include "nlohmann/json.hpp"

void LogicServiceImpl::Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                               ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->token());
    try {
        auto jsonbody = nlohmann::json::parse(request->token());
        auto it = jsonbody.find("mid");
        int mid;
        if (it != jsonbody.end() && it->is_number()) {
            mid = it.value();
            response->set_mid(mid);
        }
        response->set_roomid(jsonbody["room_id"]);
        response->set_key(std::to_string(mid));
        std::vector<int> accepts = jsonbody["accepts"];
        for (auto it : accepts) {
            response->add_accepts(it);
        }
        auto serverid = jsonbody["server"];
        addServerMap(jsonbody["mid"],std::to_string(mid),request->server());
    } catch (std::exception& e) {
        LOG_TRACE("parse json error");
    }
    done->Run();
}

void LogicServiceImpl::addServerMap(const int mid, const std::string& key, const std::string& server){
    redisclient_.Execute();
}
