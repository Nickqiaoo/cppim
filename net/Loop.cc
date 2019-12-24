#include "Loop.h"

Loop::Loop() {
    ios_ = std::make_shared<asio::io_service>();
    work_ = std::make_shared<asio::io_service::work>(*ios_);
}
Loop::~Loop() {}
void Loop::start() { thread_ = std::thread(&Loop::run, this); }
void Loop::run() { ios_->run(); }
void Loop::stop() {
    ios_->stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}
void Loop::runInLoop(const Callback& cb) { ios_->post(cb); }