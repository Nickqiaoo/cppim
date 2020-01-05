#include "GateServer.h"

GateServer::GateServer(LoopPtr loop, int thrnum, const std::string& tcpip, int tcpport, const std::string& rpcip, int rpcport)
    : tcpserver_(thrnum, tcpip, tcpport),
      rpcserver_(thrnum, rpcip, rpcport),
      rpcclient_(loop),
      clientcodec_(std::bind(&GateServer::onClientMessageCallback, this, _1, _2, _3, _4)) {
          tcpserver_.setMessageCallback(std::bind(&ClientCodec::onMessage,&clientcodec_,_1,_2));
      }
GateServer::~GateServer() {}

void GateServer::onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body) {
    switch (op) {
        case 7:
            logic::ConnectReq* req = new logic::ConnectReq;
            req->set_server(serverid_);
            req->set_token(body);
            rpcclient_.Connect(req, session);
            break;

        default:
            break;
    }
}

void GateServer::Start() {
    tcpserver_.start();
    rpcserver_.registerService(&gateservice_);
    rpcserver_.start();
}

void GateServer::HandleConnect(logic::ConnectReply* response, const SessionPtr session) {
    int64_t mid = response->mid();
    std::string key = response->key();
    std::string roomid = response->roomid();
    for (int i = 0; i < response->accepts_size(); i++) {
    }
    clientcodec_.send(session, 8, 0, std::string());
}