#pragma once

#include "Session.h"

class UserSession : public Session {
   public:
    explicit UserSession(Loop* loop, int64_t id) : Session(loop, id) {}
    ~UserSession() {}

    std::string getKey() const { return key_; }
    void setKey(const std::string& key) { key_ = key; }

    int32_t getMid() const { return mid_; }
    void setMid(int32_t mid) { mid_ = mid; }

    std::string getRoom() const { return room_; }
    void setRoom(const std::string& room) { room_ = room; }

   private:
    std::string key_;
    int32_t mid_;
    std::string room_;
};