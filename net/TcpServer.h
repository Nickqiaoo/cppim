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
    void start();
    void setMessageCallback(const onMessageCallback& cb) { messagecallback_ = cb; }
    void setConnectionCallback(const onConnectionCallback& cb) { connectioncallback_ = cb; }

   private:
    template <typename T>
    SessionPtr newSession();

   private:
    std::map<int, SessionPtr> connections_;
    uint64_t sessionid_{0};
    LoopMgr loopmgr_;
    Acceptor acceptor_;
    onConnectionCallback connectioncallback_;
    onMessageCallback messagecallback_;
};