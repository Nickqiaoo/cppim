#include "GateServer.h"

void LogicRpcClient::Connect(logic::ConnectReq* request, logic::ConnectReply* response, google::protobuf::Closure* done) {
        //logic::ConnectReply* response = new logic::ConnectReply;
        stub_.Connect(NULL, request, response, done);
    }