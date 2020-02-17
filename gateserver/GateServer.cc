#include "GateServer.h"
#include "UserSession.h"

GateServer::GateServer(int thrnum, const std::string& tcpip, int tcpport, const std::string& rpcip, int rpcport, const std::string& clientip,
                       int clientport, const std::string& serverid)
    : tcpserver_(thrnum, tcpip, tcpport),
      rpcserver_(thrnum, rpcip, rpcport),
      rpcclient_(clientip, clientport, this, &loop_),
      gateservice_(this),
      clientcodec_(std::bind(&GateServer::onClientMessageCallback, this, _1, _2, _3, _4)),
      serverid_(serverid) {
    tcpserver_.setMessageCallback(std::bind(&ClientCodec::onMessage, &clientcodec_, _1, _2));
    tcpserver_.setServerDisconnectCallback(std::bind(&GateServer::HandleDisconnect, this, _1));
}
GateServer::~GateServer() {}

void GateServer::onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body) {
    auto usersession = static_pointer_cast<UserSession>(session);
    // FIXME:handle client_ null
    switch (op) {
        case 7: {
            LOG_INFO("client onmessage token: {}", body);
            logic::ConnectReq* request = new logic::ConnectReq;
            logic::ConnectReply* response = new logic::ConnectReply;
            request->set_server(serverid_);
            request->set_token(body);
            rpcclient_.Connect(request, response, ::google::protobuf::internal::NewCallback(this, &GateServer::HandleConnect, response, session));
        } break;

        case 2: {
            LOG_INFO("client heartbeat");
            logic::HeartbeatReq* request = new logic::HeartbeatReq;
            logic::HeartbeatReply* response = new logic::HeartbeatReply;
            request->set_server(serverid_);
            request->set_mid(usersession->getMid());
            request->set_key(usersession->getKey());
            rpcclient_.HeartBeat(request, response, ::google::protobuf::internal::NewCallback(this, &GateServer::HandleHeartbeat, response, session));
        } break;

        default:
            clientcodec_.send(session, 1, 1, "test");
            break;
    }
}

void GateServer::Start() {
    tcpserver_.setNewSessionCallback<UserSession>();
    tcpserver_.start();
    rpcserver_.registerService(&gateservice_);
    rpcserver_.start();
    rpcclient_.connect();
    loop_.start();
}

void GateServer::Stop() {
    tcpserver_.stop();
    rpcserver_.stop();
    loop_.stop();
}

void GateServer::HandleConnect(logic::ConnectReply* response, const SessionPtr session) {
    int64_t mid = response->mid();
    std::string key = response->key();
    std::string roomid = response->roomid();
    auto usersession = static_pointer_cast<UserSession>(session);
    usersession->setKey(key);
    usersession->setMid(mid);
    usersession->setRoom(roomid);
    for (int i = 0; i < response->accepts_size(); i++) {
    }
    LOG_INFO("handle connect mid:{} key:{} romid:{}", mid, key, roomid);
    channels_.insert({key, session});
    // LOG_INFO("session size:{}", channels_.size());
    rooms_[roomid].insert(session);
    clientcodec_.send(session, 8, 0, std::string("success"));
}

void GateServer::HandleHeartbeat(logic::HeartbeatReply* response, const SessionPtr session) { LOG_INFO("handle heartbeat reply"); }

void GateServer::HandleDisconnect(const SessionPtr session) {
    auto usersession = static_pointer_cast<UserSession>(session);
    LOG_INFO("handle disconnect mid:{} key:{}", usersession->getMid(), usersession->getKey());

    logic::DisconnectReq* request = new logic::DisconnectReq;
    logic::DisconnectReply* response = new logic::DisconnectReply;
    request->set_server(serverid_);
    request->set_mid(usersession->getMid());
    request->set_key(usersession->getKey());
    rpcclient_.Disconnect(request, response, nullptr);
    
    auto it = channels_.find(usersession->getKey());
    if (it != channels_.end()) {
        channels_.erase(it);
        //LOG_INFO("erase channel session key:{} channel size:{}", usersession->getKey(), channels_.size());
    }
    auto roomit = rooms_.find(usersession->getRoom());
    if (roomit != rooms_.end()) {
        auto& roomsession = roomit->second;
        roomsession.erase(session);
        LOG_INFO("erase room session key:{} room size:{}", usersession->getKey(), roomsession.size());
    }
}

void GateServer::SendToClient(const std::string& key, const gate::Proto& msg) {
    auto it = channels_.find(key);
    if (it != channels_.end()) {
        LOG_INFO("send message to key:{}", key);
        clientcodec_.send(it->second, msg.op(), msg.seq(), msg.body());
    }
}
void GateServer::SendToRoom(const std::string& roomid, const gate::Proto& msg) {
    auto it = rooms_.find(roomid);
    if (it != rooms_.end()) {
        for (auto session : it->second) {
            clientcodec_.send(session, msg.op(), msg.seq(), msg.body());
        }
    }
}
void GateServer::BroadCast(const gate::Proto& msg) {
    for (auto session : channels_) {
        clientcodec_.send(session.second, msg.op(), msg.seq(), msg.body());
    }
}