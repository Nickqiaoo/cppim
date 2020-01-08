#include "JobServer.h"

void GateRpcClient::PushMsg(gate::PushMsgReq* request) {
        gate::PushMsgReply* response = new gate::PushMsgReply;
        stub_.PushMsg(NULL, request, response, NewCallback(jobserver_, &JobServer::HandlePushMsg, response));
    }