#include "GateServer.h"

void LogicRpcClient::Connect(logic::ConnectReq* request, logic::ConnectReply* response, google::protobuf::Closure* done) {
    // logic::ConnectReply* response = new logic::ConnectReply;
    stub_.Connect(NULL, request, response, done);
}

void LogicRpcClient::HeartBeat(logic::HeartbeatReq* request, logic::HeartbeatReply* response, google::protobuf::Closure* done) {
    stub_.Heartbeat(NULL, request, response, done);
}

void LogicRpcClient::Disconnect(logic::DisconnectReq* request, logic::DisconnectReply* response, google::protobuf::Closure* done) {
    stub_.Disconnect(NULL, request, response, done);
}