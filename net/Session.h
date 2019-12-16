#pragma once

#include "net_define.h"

class Session {
  explicit Session(asio::io_service &ios);
  ~Session();
  Session(const Session &) = delete;
  Session operator=(const Session &) = delete;

 public:
  asio::ip::tcp::socket &Socket() { return socket_; }

 private:
  asio::ip::tcp::socket socket_;
};