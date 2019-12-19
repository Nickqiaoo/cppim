#include "Acceptor.h"
#include "Loop.h"
#include "Session.h"

Acceptor::Acceptor(const LoopPtr &loop, const std::string &ip, int port)
    : loop_(loop), ip_(ip), port_(port), acceptor_(loop->ios()) {}

Acceptor::~Acceptor() {}
void Acceptor::start() { loop_->runInLoop(std::bind(&Acceptor::listen, this)); }

void Acceptor::listen() {
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip_), port_);

    acceptor_.open(ep.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.non_blocking(true);

    acceptor_.bind(ep);
    acceptor_.listen();
    accept();
}
void Acceptor::accept() {
    auto session = newSessionCallback_();
    acceptor_.async_accept(session->Socket(), [session, this](const asio::error_code &err) {
        if(!acceptor_.is_open()){
            return ;
        }
        if (!err) {
            session->start();
        } else {
        }
        accept();
    });
}
void Acceptor::close(){
    acceptor_.close();
}