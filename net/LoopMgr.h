#pragma once

#include <atomic>
#include <mutex>
#include "net_define.h"

class LoopMgr{
    public:
    explicit LoopMgr(size_t thrnum);
    ~LoopMgr();

    void start();
    void stop();
    Loop* findNextLoop();
    private:
    std::mutex mutex_;
    std::vector<Loop*> loops_;
    std::atomic<size_t> next_{0};
};