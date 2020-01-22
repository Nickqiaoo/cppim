#include "JobServer.h"

void GateRpcClient::PushMsg(gate::PushMsgReq* request, gate::PushMsgReply* response, ::google::protobuf::Closure* done) {
    // gate::PushMsgReply* response = new gate::PushMsgReply;
    stub_.PushMsg(NULL, request, response, done);
}
void GateRpcClient::BroadcastRoom(gate::BroadcastRoomReq* request, gate::BroadcastRoomReply* response, ::google::protobuf::Closure* done) {
    stub_.BroadcastRoom(NULL, request, response, done);
}
void GateRpcClient::Broadcast(gate::BroadcastReq* request, gate::BroadcastReply* response, ::google::protobuf::Closure* done) {
    stub_.Broadcast(NULL, request, response, done);
}