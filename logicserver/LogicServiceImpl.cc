#include "LogicServiceImpl.h"

void LogicServiceImpl::Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                          ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->token());
    response->set_key("test");
    response->set_mid(1);
    response->set_roomid("room1");
    done->Run();
}