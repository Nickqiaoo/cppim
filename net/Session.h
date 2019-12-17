#pragma once
#include <memory>

#include "../Buffer.h"
#include "net_define.h"

class Session : public std::enable_shared_from_this<Session> {
  public:
    explicit Session(LoopPtr loop, uint64_t id);
    ~Session();
    Session(const Session &) = delete;
    Session operator=(const Session &) = delete;

  public:
    asio::ip::tcp::socket &Socket() { return socket_; }
    void start();

  private:
    void read();

  private:
    uint64_t id_;
    LoopPtr loop_;
    Buffer readbuf_;
    Buffer writebuf_;
    asio::ip::tcp::socket socket_;
};