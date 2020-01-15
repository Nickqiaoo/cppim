#include "GateServer.h"

GateServer::GateServer(LoopPtr loop, int thrnum, const std::string& tcpip, int tcpport, const std::string& rpcip, int rpcport,
                       const std::string& clientip, int clientport)
    : tcpserver_(thrnum, tcpip, tcpport),
      rpcserver_(thrnum, rpcip, rpcport),
      rpcclient_(clientip, clientport, this, loop),
      gateservice_(this),
      clientcodec_(std::bind(&GateServer::onClientMessageCallback, this, _1, _2, _3, _4)) {
    tcpserver_.setMessageCallback(std::bind(&ClientCodec::onMessage, &clientcodec_, _1, _2));
}
GateServer::~GateServer() {}

void GateServer::onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body) {
    switch (op) {
        case 7: {
            LOG_INFO("client onmessage token: {}", body);
            logic::ConnectReq* req = new logic::ConnectReq;
            req->set_server(serverid_);
            req->set_token(body);
            rpcclient_.Connect(req, session);
        } break;

        default:
            clientcodec_.send(session,1,1,"test");
            break;
    }
}

void GateServer::Start() {
    tcpserver_.start();
    rpcserver_.registerService(&gateservice_);
    rpcserver_.start();
    rpcclient_.connect();
}

void GateServer::HandleConnect(logic::ConnectReply* response, const SessionPtr session) {
    int64_t mid = response->mid();
    std::string key = response->key();
    std::string roomid = response->roomid();
    for (int i = 0; i < response->accepts_size(); i++) {
    }
    LOG_INFO("handle connect mid:{} key:{} romid:{}", mid, key, roomid);
    channels_.insert({key, session});
    //LOG_INFO("session size:{}", channels_.size());
    rooms_[roomid].insert(session);
    clientcodec_.send(session, 8, 0, std::string("success"));
}

void GateServer::SendToClient(const std::string& key, const gate::Proto& msg) {
    auto it = channels_.find(key);
    if (it != channels_.end()) {
        LOG_INFO("send message to key:{}",key);
        clientcodec_.send(it->second, msg.op(), msg.seq(), msg.body());
    }
}