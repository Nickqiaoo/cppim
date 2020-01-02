#include "RpcServer.h"
#include "gate.pb.h"
#include "log.h"

class GateServiceImpl : public gate::Gate {
   public:
    virtual void PushMsg(::google::protobuf::RpcController* controller, const ::gate::PushMsgReq* request, ::gate::PushMsgReply* response,
                         ::google::protobuf::Closure* done) override {
        LOG_INFO("receive msg:{}", request->msg());
        done->Run();
    }
};