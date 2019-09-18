#pragma once

#include <asio.hpp>
#include <memory>

#include "Buffer.h"
#include "HttpContext.h"

class HttpSession : public std::enable_shared_from_this<HttpSession> {

  public:
    HttpSession(const HttpSession &) = delete;
    HttpSession operator=(const HttpSession &) = delete;
    
    ~HttpSession();
    explicit HttpSession(asio::io_service &service);

    asio::ip::tcp::socket &Socket() { return socket_; }
    void Start();
    void Send(Buffer* buf);

  private:
    Buffer readbuf_;
    Buffer writebuf_;
    HttpContext context_;
    asio::ip::tcp::socket socket_;
};