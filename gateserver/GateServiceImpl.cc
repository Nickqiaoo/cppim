#include "GateServiceImpl.h"
#include "GateServer.h"

void GateServiceImpl::PushMsg(::google::protobuf::RpcController* controller, const ::gate::PushMsgReq* request, ::gate::PushMsgReply* response,
                              ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->proto().body());
    for (int i = 0; i < request->keys_size(); i++) {
        // LOG_INFO("send message to key:{}",request->keys(i));
        gateserver_->SendToClient(request->keys(i), request->proto());
    }
    done->Run();
}

void GateServiceImpl::Broadcast(::google::protobuf::RpcController* controller, const ::gate::BroadcastReq* request, ::gate::BroadcastReply* reply,
                                ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->proto().body());
    gateserver_->BroadCast(request->proto());
    done->Run();
}
void GateServiceImpl::BroadcastRoom(::google::protobuf::RpcController* controller, const ::gate::BroadcastRoomReq* request, ::gate::BroadcastRoomReply* reply,
                                    ::google::protobuf::Closure* done) {
    LOG_INFO("receive msg:{}", request->proto().body());
    gateserver_->SendToRoom(request->roomid(), request->proto());
    done->Run();
}