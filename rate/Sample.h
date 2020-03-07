#pragma once

#include <atomic>

#include "log.h"

class Sample {
   public:
    Sample() : count_(0), maxInFlight_(0), drop_(0), totalRTT_(0) {}
    ~Sample() {}

    void Add(int64_t rtt, int64_t inflight, bool drop) {
        LOG_INFO("Add rtt:{} inflight:{} count:{} drop:{}",rtt, inflight, count_, drop);
        if (drop) {
            drop_.store(1);
        }
        for (auto max = maxInFlight_.load(); max < inflight; max = maxInFlight_.load()) {
            if (maxInFlight_.compare_exchange_strong(max, inflight)) {
                break;
            }
        }

        count_++;
        totalRTT_ += rtt;
    }

    int64_t RTT() {
        int64_t count = count_.load();
        if (count == 0) {
            return 0;
        }
        LOG_INFO("totalRTT:{} count:{}",totalRTT_.load(), count);
        return totalRTT_.load() / count;
    }

    int64_t MaxInFlight() { return maxInFlight_.load(); }

    int64_t Count() { return count_.load(); }

    int64_t Drop() { return drop_.load(); }

   private:
    std::atomic<int64_t> count_;
    std::atomic<int64_t> maxInFlight_;
    std::atomic<int64_t> drop_;
    std::atomic<int64_t> totalRTT_;
};