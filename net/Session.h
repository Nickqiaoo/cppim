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
    void setMessageCallback(const onMessageCallback& cb){
        messagecallback_ = cb;
    }

  private:
    void read();
    void send();
  private:
    uint64_t id_;
    LoopPtr loop_;
    Buffer readbuf_;
    Buffer writebuf_;
    asio::ip::tcp::socket socket_;
    std::vector<BufferPtr> unsend_buf_;
    std::vector<BufferPtr> tmp_buf_;
    onMessageCallback messagecallback_;
};