#pragma once;

#include <functional>
#include "Session.h"
#include "log.h"
#include "net_define.h"

class ClientCodec {
   public:
    typedef std::function<void(const SessionPtr&, int op, int id, const std::string& body)> ClientMessageCallback;

    explicit ClientCodec(const ClientMessageCallback& cb) : messageCallback_(cb) {}

    void onMessage(const SessionPtr conn, BufferPtr buf) {
        while (buf->readableBytes() >= kHeaderLen)  // kHeaderLen == 4
        {
            // FIXME: use Buffer::peekInt32()
            const int32_t len = buf->peekInt32();
            if (len > 65536 || len < 0) {
                LOG_ERROR("Invalid length");
                conn->close();  // FIXME: disable reading
                break;
            } else if (buf->readableBytes() >= len + kHeaderLen) {
                buf->retrieve(kHeaderLen);
                int op, id;
                std::string body;
                muduo::string message(buf->peek(), len);
                messageCallback_(conn, op, id, body);
                buf->retrieve(len);
            } else {
                break;
            }
        }
    }

    // FIXME: TcpConnectionPtr
    void send(const SessionPtr conn, int op, int id, const std::string& body) {
        LOG_INFO("send messageid {}");
        auto buf = std::make_shared<Buffer>();
        fillEmptyBuffer(buf, op, id, body);
        conn->send(buf);
    }

   private:
    ClientMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};