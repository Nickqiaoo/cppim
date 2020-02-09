#pragma once
#include "net_define.h"
#include <thread>

class Loop {
    typedef std::shared_ptr<asio::io_service> iosptr;
    typedef std::shared_ptr<asio::io_service::work> workptr;

  public:
    explicit Loop(std::string str="");
    ~Loop();

    void start();
    void stop();
    void runInLoop(const Callback& cb);
    asio::io_service& ios(){
      return ios_;
    }

  private:
    void run();

  private:
    std::string test_;
    asio::io_service ios_;
    asio::io_service::work work_;
    std::thread thread_;
};