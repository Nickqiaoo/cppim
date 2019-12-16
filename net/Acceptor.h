#pragma once
#include "net_define.h"
#include <string>
#include <functional>


class Acceptor {
  public:
    explicit Acceptor(const LoopPtr& loop, const std::string& ip, int port);
    ~Acceptor();

    bool start();
    bool stop();
    void setNewSessionCallback(const NewSessionCallback& cb){
      newSessionCallback_ = cb;
    }

  private:
    void listen();
    void accept();
    void close();

  private:
    LoopPtr loop_;
    asio::ip::tcp::acceptor acceptor_;
    NewSessionCallback newSessionCallback_;
};