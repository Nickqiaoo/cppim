#include "Acceptor.h"
#include "Loop.h"
#include "Session.h"

Acceptor::Acceptor(const LoopPtr& loop, const std::string& ip, int port)
    : loop_(loop),
      acceptor_(loop->getios(), asio::ip::tcp::endpoint(
                                    asio::ip::address::from_string(ip), port)) {
}
bool Acceptor::start() {
  listen();
  loop_->runInLoop(std::bind(&Acceptor::accept, this));
}
void Acceptor::listen() { acceptor_.listen(); }
void Acceptor::accept() {
  auto session = newSessionCallback_();
  acceptor_.async_accept(session->Socket(),
                         [session, this](const asio::error_code& err) {
                           if (err) {
                           }
                           accept();
                         });
    
}