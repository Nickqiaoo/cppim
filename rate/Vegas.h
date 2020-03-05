#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <memory>
#include <mutex>

#include "Sample.h"
#include "log.h"

enum Op { Ignore = 0, Success, Drop };

typedef std::function<void(int64_t start, Op)> Done;

struct Status {
    int64_t limit_;
    int64_t inFlight_;
    int64_t minRTT_;
    int64_t lastRTT_;
};

class Vegas {
   public:
    Vegas() : sample_(std::make_shared<Sample>()) { std::srand(std::time(nullptr)); }
    ~Vegas() {}

    Status Stat() { return Status{limit_.load(), inFlight_.load(), minRTT_ / (1000 * 1000), sample_->RTT() / (1000 * 1000)}; }

    std::pair<Done, bool> Acquire() {
        bool success = false;
        inFlight_++;
        int64_t inFlight = 1 + inFlight_.load();
        if (inFlight <= limit_.load()) {
            success = true;
        }

        return std::make_pair(
            [=](int64_t start, Op op) {
                LOG_INFO("do Done");
                inFlight_--;
                if (op == Ignore) {
                    return;
                }
                auto end = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                LOG_INFO("start:{} end:{}",start,end);
                int64_t rtt = end - start;
                auto s = sample_;
                if (op == Drop) {
                    s->Add(rtt, inFlight, true);
                } else if (op == Success) {
                    s->Add(rtt, inFlight, false);
                }
                if (end > updateTime_.load() && s->Count() >= 16) {
                    std::lock_guard<std::mutex> mutex(mu_);
                    LOG_INFO("updateTime:{} Count:{}",updateTime_.load(),s->Count());
                    if (sample_ != s) {
                        return;
                    }
                    sample_ = std::make_shared<Sample>();

                    int64_t lastRTT = s->RTT();
                    if (lastRTT <= 0) {
                        return;
                    }
                    int64_t updateTime = end + lastRTT * 5;
                    if (lastRTT * 5 < minWindowTime_) {
                        updateTime = end + minWindowTime_;
                    } else if (lastRTT * 5 > maxWindowTime_) {
                        updateTime = end + maxWindowTime_;
                    }
                    updateTime_.store(updateTime);
                    int64_t limit = limit_.load();
                    int64_t queue = double(limit) * (1 - double(minRTT_) / double(lastRTT));
                    LOG_INFO("queue:{}",queue);
                    probes_--;
                    if (probes_ <= 0) {
                        int64_t maxFlight = s->MaxInFlight();
                        if (maxFlight * 2 < limit_ || maxFlight <= minLimit_) {
                            probes_ = 3 * limit + std::rand() % (3 * limit);
                            minRTT_ = lastRTT;
                        }
                    }
                    if (minRTT_ == 0 || lastRTT < minRTT_) {
                        minRTT_ = lastRTT;
                    }
                    double newLimit;
                    double threshold = std::sqrt(double(limit)) / 2;
                    if (s->Drop()) {
                        newLimit = double(limit) - threshold;
                    } else if (s->MaxInFlight() * 2 < limit) {
                        return;
                    } else {
                        if (queue < threshold) {
                            newLimit = double(limit) + 6 * threshold;
                        } else if (queue < 2 * threshold) {
                            newLimit = double(limit) + 3 * threshold;
                        } else if (queue < 3 * threshold) {
                            newLimit = double(limit) + threshold;
                        } else if (queue > 6 * threshold) {
                            newLimit = double(limit) - threshold;
                        } else {
                            return;
                        }
                    }
                    newLimit = std::max(minLimit_, std::min(maxLimit_, (int64_t)newLimit));
                    limit_.store(newLimit);
                }
            },
            success);
    }

   private:
    const int64_t minWindowTime_ = 5 * 1000 * 1000; //500ms
    const int64_t maxWindowTime_ = 2000 * 1000 * 1000; // 2000ms
    const int64_t minLimit_ = 50;
    const int64_t maxLimit_ = 4096;
    
    std::atomic<int64_t> limit_{minLimit_};
    std::atomic<int64_t> inFlight_{0};
    std::atomic<int64_t> updateTime_{0};
    int64_t minRTT_{0};

    std::shared_ptr<Sample> sample_;
    std::mutex mu_;
    int64_t probes_{100};
};
