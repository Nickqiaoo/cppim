#include "Session.h"
#include "RpcChannel.h"

class RpcSession : public Session {
   public:
    explicit RpcSession(Loop* loop, uint64_t id) : Session(loop, id) {}
    virtual ~RpcSession() override {delete channel_;}
    void setChannel(RpcChannel* channel) { channel_ = channel; }

   private:
    RpcChannel* channel_{nullptr};
};