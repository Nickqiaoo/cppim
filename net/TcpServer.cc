#include "TcpServer.h"
#include <string>

TcpServer::TcpServer(int thrnum, const std::string &ip, int port)
    : loopmgr_(thrnum), acceptor_(loopmgr_.findNextLoop(), ip, port) {
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession, this));
}
TcpServer::~TcpServer(){}
SessionPtr TcpServer::newSession() {
    auto loop = loopmgr_.findNextLoop();
    auto session = std::make_shared<Session>(loop, sessionid_++);
    session->setMessageCallback(messagecallback_);
    session->setConnectionCallback(connectioncallback_);
    connections_.insert({sessionid_, session});
    return session;
}

void TcpServer::start(){
    acceptor_.start();
    loopmgr_.start();
}