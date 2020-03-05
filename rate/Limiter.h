#pragma once
#include "Vegas.h"

class Limiter {
    typedef std::function<void(Op)> Func;

   public:
    std::pair<Func, bool> Allow() {
        auto res = vegas_.Acquire();
        if (res.second == false) {
            res.first(0, Ignore);
            return std::make_pair(nullptr, false);
        }
        int64_t start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return std::make_pair([=](Op op){
            res.first(start, op);
        }, true);
    }

    Status Stat(){
        return vegas_.Stat();
    }

   private:
    Vegas vegas_;
};