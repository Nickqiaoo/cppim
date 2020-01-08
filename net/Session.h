#pragma once
#include <memory>
#include <mutex>

#include "asio/steady_timer.hpp"
#include "Buffer.h"
#include "net_define.h"

class Session : public std::enable_shared_from_this<Session> {
  public:
    explicit Session(LoopPtr loop, uint64_t id = 0);
    ~Session();
    Session(const Session &) = delete;
    Session operator=(const Session &) = delete;

  public:
    asio::ip::tcp::socket &Socket() { return socket_; }
    void start();
    void setMessageCallback(const onMessageCallback& cb){
        messagecallback_ = cb;
    }
    void setConnectionCallback(const onConnectionCallback& cb){
      connectioncallback_ = cb;
    }

    void send(BufferPtr buffer);
    void connect(const string& ip, int port);
    void close();
  private:
    void read();
    void write();
  private:
    uint64_t id_;
    std::mutex mutex_;
    LoopPtr loop_;
    Buffer read_buf_;
    Buffer write_buf_;
    asio::ip::tcp::socket socket_;
    asio::steady_timer timer_;
    std::vector<BufferPtr> unsend_queue_;
    std::vector<BufferPtr> tmp_queue_;
    onMessageCallback messagecallback_;
    onConnectionCallback connectioncallback_;
};