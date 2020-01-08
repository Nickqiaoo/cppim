#include "GateServiceImpl.h"
#include "GateServer.h"

void GateServiceImpl::PushMsg(::google::protobuf::RpcController* controller, const ::gate::PushMsgReq* request, ::gate::PushMsgReply* response,
                              ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->proto().body());
    for(int i = 0;i<request->keys_size();i++){
        gateserver_->SendToClient(request->keys(i),request->proto());
    }
    done->Run();
}