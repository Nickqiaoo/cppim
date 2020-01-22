#include "TcpServer.h"
#include <string>
#include "HttpSession.h"
#include "RpcSession.h"
#include "UserSession.h"
#include "log.h"

using namespace std::placeholders;

TcpServer::TcpServer(int thrnum, const std::string &ip, int port) : loopmgr_(thrnum), acceptor_(loopmgr_.findNextLoop(), ip, port) {
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<Session>, this));
}
TcpServer::~TcpServer() {}

void TcpServer::setNewUserSessionCallback() { acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<UserSession>, this)); }

void TcpServer::setNewRpcSessionCalback() { acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<RpcSession>, this)); }

void TcpServer::setNewHttpSessionCalback() { acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<HttpSession>, this)); }

void TcpServer::DefaultDisconnectCallback(int id) {
    SessionPtr session;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        // LOG_INFO("connection disconnect, size:{}", connections_.size());
        auto it = connections_.find(id);
        if (it != connections_.end()) {
            session = it->second;
            connections_.erase(id);
        }
    }
    if (disconnectcallback_) {
        if(session){
            disconnectcallback_(session);
        }
    }
}

template <typename T>
SessionPtr TcpServer::newSession() {
    auto loop = loopmgr_.findNextLoop();
    auto session = std::make_shared<T>(loop, sessionid_++);
    session->setMessageCallback(messagecallback_);
    session->setConnectionCallback(connectioncallback_);
    session->setDisconnectCallback(std::bind(&TcpServer::DefaultDisconnectCallback, this, _1));
    {
        std::lock_guard<std::mutex> guard(mutex_);
        connections_.insert({sessionid_, session});
    }
    return session;
}

void TcpServer::start() {
    acceptor_.start();
    loopmgr_.start();
}

void TcpServer::stop(){
    acceptor_.close();
    loopmgr_.stop();
}