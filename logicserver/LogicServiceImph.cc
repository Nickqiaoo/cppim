#include "LogicServiceImpl.h"

void LogicServiceImpl::Connect(::google::protobuf::RpcController* controller, const ::logic::ConnectReq* request, ::logic::ConnectReply* response,
                          ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->token());
    done->Run();
}