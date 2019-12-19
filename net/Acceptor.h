#pragma once
#include "net_define.h"
#include <string>
#include <functional>

class Acceptor {
  public:
    explicit Acceptor(const LoopPtr& loop, const std::string& ip, int port);
    ~Acceptor();

    void start();
    void close();
    void setNewSessionCallback(const NewSessionCallback& cb){
      newSessionCallback_ = cb;
    }

  private:
    void listen();
    void accept();

  private:
    LoopPtr loop_;
    asio::ip::tcp::acceptor acceptor_;
    std::string ip_;
    int port_;
    NewSessionCallback newSessionCallback_;
};