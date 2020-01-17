#include "JobServer.h"

void GateRpcClient::PushMsg(gate::PushMsgReq* request, gate::PushMsgReply* response, ::google::protobuf::Closure* done) {
        //gate::PushMsgReply* response = new gate::PushMsgReply;
        stub_.PushMsg(NULL, request, response, done);
    }