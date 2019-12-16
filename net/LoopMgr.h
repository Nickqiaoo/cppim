#pragma once

#include <mutex>
#include "net_define.h"

class LoopMgr{
    public:
    explicit LoopMgr(size_t thrnum);
    ~LoopMgr();

    void start();
    void destory();
    LoopPtr findNextLoop();
    private:
    std::mutex mutex_;
    std::vector<LoopPtr> loops_;
    size_t next_;
};