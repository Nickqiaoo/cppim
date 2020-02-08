#pragma once
#include "net_define.h"
#include <string>
#include <functional>

class Acceptor {
  public:
    explicit Acceptor(Loop* loop, const std::string& ip, int port);
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
    Loop* loop_;
    std::string ip_;
    int port_;
    asio::ip::tcp::acceptor acceptor_;
    NewSessionCallback newSessionCallback_;
};