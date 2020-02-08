#pragma once

#include "Session.h"

class UserSession : public Session {
   public:
    explicit UserSession(Loop* loop, int64_t id) : Session(loop, id) {}
    ~UserSession() {}

    std::string getKey() const { return key; }

    int32_t getMid() const { return mid; }

    std::string getRoom() const { return room; }

   private:
    std::string key;
    int32_t mid;
    std::string room;
};