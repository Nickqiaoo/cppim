#pragma once

#include "HttpContext.h"
#include "Session.h"

class HttpSession : public Session {
   public:
    explicit HttpSession(Loop* loop, uint64_t id) : Session(loop, id) {}
    ~HttpSession() {}
    HttpContext* getContext() { return &context_; }

   private:
    HttpContext context_;
};