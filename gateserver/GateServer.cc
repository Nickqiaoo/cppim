#include "GateServer.h"

void GateServer::onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body){
    switch (op){
    case 7:
        logic::ConnectReq* req = new logic::ConnectReq;
        rpcclient_.Connect(req, session);
        break;
    
    default:
        break;
    }
}