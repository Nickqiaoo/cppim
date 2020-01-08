#include "TcpServer.h"
#include "RpcSession.h"
#include "HttpSession.h"
#include <string>

TcpServer::TcpServer(int thrnum, const std::string &ip, int port)
    : loopmgr_(thrnum), acceptor_(loopmgr_.findNextLoop(), ip, port) {
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<Session>, this));
}
TcpServer::~TcpServer(){}

void TcpServer::setNewRpcSessionCalback(){
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<RpcSession>, this));
}

void TcpServer::setNewHttpSessionCalback(){
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<HttpSession>, this));
}

template<typename T>
SessionPtr TcpServer::newSession() {
    auto loop = loopmgr_.findNextLoop();
    auto session = std::make_shared<T>(loop, sessionid_++);
    session->setMessageCallback(messagecallback_);
    session->setConnectionCallback(connectioncallback_);
    connections_.insert({sessionid_, session});
    return session;
}

void TcpServer::start(){
    acceptor_.start();
    loopmgr_.start();
}