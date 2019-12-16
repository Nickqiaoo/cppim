#include "loop.h"

Loop::Loop() : thread_(&Loop::run) {
  ios_ = std::make_shared<asio::io_service>();
  work_ = std::make_shared<asio::io_service::work>(*ios_);
}

void Loop::run() { ios_->run(); }

void Loop::runInLoop(const Callback& cb){
    ios_->post(cb);
}