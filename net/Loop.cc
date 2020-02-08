#include "Loop.h"
#include "log.h"

Loop::Loop(std::string str) :test_(str),work_(ios_){
    LOG_INFO("loop create");
    
}
Loop::~Loop() {
    LOG_INFO("loop destory:{}",test_);
}
void Loop::start() { thread_ = std::thread(&Loop::run, this); }
void Loop::run() { ios_.run(); }
void Loop::stop() {
    ios_.stop();
    if (thread_.joinable()) {
        thread_.join();
    }
}
void Loop::runInLoop(const Callback& cb) { ios_.post(cb); }