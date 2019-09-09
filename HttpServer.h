#pragma once

#include "HttpSession.h"

class HttpServer {
    typedef std::shared_ptr<HttpSession> HttpSessionPtr;

  public:
    HttpServer(asio::io_service, asio::ip::tcp::endpoint ep);
    void doAccept();
    void Start();
  private:
    asio::io_service &io_service_;
    asio::ip::tcp::acceptor acceptor_;
};