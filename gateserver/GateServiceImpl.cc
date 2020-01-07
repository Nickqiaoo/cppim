#include "GateServiceImpl.h"

void GateServiceImpl::PushMsg(::google::protobuf::RpcController* controller, const ::gate::PushMsgReq* request, ::gate::PushMsgReply* response,
                              ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->proto());
    
    done->Run();
}