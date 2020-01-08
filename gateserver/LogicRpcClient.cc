#include "GateServer.h"

void LogicRpcClient::Connect(logic::ConnectReq* request, SessionPtr session) {
        logic::ConnectReply* response = new logic::ConnectReply;
        stub_.Connect(NULL, request, response, NewCallback(gateserver_, &GateServer::HandleConnect, response, session));
    }