#pragma once
#include <map>
#include "Acceptor.h"
#include "LoopMgr.h"
#include "Session.h"
#include "net_define.h"

class TcpServer {
   public:
    TcpServer(int thrnum, const std::string& ip, int port);
    ~TcpServer();

    template <typename T>
    void setNewSessionCallback() {
        acceptor_.setNewSessionCallback(std::bind(&TcpServer::newSession<T>, this));
    }
    void start();
    void stop();
    void setMessageCallback(const onMessageCallback& cb) { messagecallback_ = cb; }
    void setConnectionCallback(const onConnectionCallback& cb) { connectioncallback_ = cb; }
    void setServerDisconnectCallback(const onServerDisconnectCallback& cb) { disconnectcallback_ = cb; }

   private:
    template <typename T>
    SessionPtr newSession() {
        auto loop = loopmgr_.findNextLoop();
        auto session = std::make_shared<T>(loop, sessionid_);
        session->setMessageCallback(messagecallback_);
        session->setConnectionCallback(connectioncallback_);
        session->setDisconnectCallback(std::bind(&TcpServer::DefaultDisconnectCallback, this, std::placeholders::_1));
        {
            std::lock_guard<std::mutex> guard(mutex_);
            connections_.insert({sessionid_, session});
        }
        sessionid_++;
        return session;
    }

    void DefaultDisconnectCallback(int id);

   private:
    LoopMgr loopmgr_;
    std::map<int, SessionPtr> connections_;
    uint64_t sessionid_{1};
    Acceptor acceptor_;
    std::mutex mutex_;
    onConnectionCallback connectioncallback_;
    onMessageCallback messagecallback_;
    onServerDisconnectCallback disconnectcallback_;
};