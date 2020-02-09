#include "TcpServer.h"
#include <string>
#include "HttpSession.h"
#include "RpcSession.h"
#include "UserSession.h"
#include "log.h"

TcpServer::TcpServer(int thrnum, const std::string &ip, int port) : loopmgr_(thrnum), acceptor_(loopmgr_.findNextLoop(), ip, port) {
    acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<Session>, this));
}
TcpServer::~TcpServer() {}

void TcpServer::DefaultDisconnectCallback(int id) {
    SessionPtr session;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        LOG_INFO("connection disconnect, size:{}", connections_.size());
        auto it = connections_.find(id);
        if (it != connections_.end()) {
            session = it->second;
            connections_.erase(id);
            LOG_INFO("session count:{}",session.use_count());
        }
    }
    if (disconnectcallback_) {
        if(session){
            disconnectcallback_(session);
        }
    }
}

void TcpServer::start() {
    acceptor_.start();
    loopmgr_.start();
}

void TcpServer::stop(){
    acceptor_.close();
    loopmgr_.stop();
}