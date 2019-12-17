#include "loop.h"

Loop::Loop() {
  ios_ = std::make_shared<asio::io_service>();
  work_ = std::make_shared<asio::io_service::work>(*ios_);
}
Loop::~Loop(){}
void Loop::start(){
  thread_ = std::move(std::thread(&Loop::run,this));
}
void Loop::run() { ios_->run(); }

void Loop::runInLoop(const Callback& cb){
    ios_->post(cb);
}