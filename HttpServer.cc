#include "HttpServer.h"
#include<iostream>

HttpServer::HttpServer(asio::io_service io_service, asio::ip::tcp::endpoint ep)
    : io_service_(io_service), acceptor_(io_service, ep) {}

HttpServer::~HttpServer() {}

void HttpServer::Start(){
    auto session = std::make_shared<HttpSession>(io_service_);
    acceptor_.async_accept(session->Socket(),[session,this](const asio::error_code& err){
        if(err){
            std::cout<<"accept err:"<<err.message()<<std::endl;
        }
        session->Start();
        Start();
    });
}