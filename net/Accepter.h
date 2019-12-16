#pragma once
#include "net_define.h"
#include <string>
#include <functional>


class Accepter {
  public:
    explicit Accepter(const LoopPtr& loop, const std::string& ip, int port);
    ~Accepter();

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
    asio::ip::tcp::acceptor accepter_;
    NewSessionCallback newSessionCallback_;
};