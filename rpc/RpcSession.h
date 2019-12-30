#include "Session.h"

class RpcSession : public Session {
   public:
    explicit RpcSession(LoopPtr loop, uint64_t id) : Session(loop, id) {}
    ~RpcSession() {}
    void setChannel(RpcChannelPtr channel) { channel_ = channel; }

   private:
    RpcChannelPtr channel_;
};