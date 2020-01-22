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

    void setNewRpcSessionCalback();
    void setNewHttpSessionCalback();
    void setNewUserSessionCallback();
    void start();
    void stop();
    void setMessageCallback(const onMessageCallback& cb) { messagecallback_ = cb; }
    void setConnectionCallback(const onConnectionCallback& cb) { connectioncallback_ = cb; }
    void setUserDisconnectCallback(const onUserDisconnectCallback& cb) { disconnectcallback_ = cb; }

   private:
    template <typename T>
    SessionPtr newSession();
    
    void DefaultDisconnectCallback(int id);

   private:
    std::map<int, SessionPtr> connections_;
    uint64_t sessionid_{0};
    LoopMgr loopmgr_;
    Acceptor acceptor_;
    std::mutex mutex_;
    onConnectionCallback connectioncallback_;
    onMessageCallback messagecallback_;
    onUserDisconnectCallback disconnectcallback_;
};