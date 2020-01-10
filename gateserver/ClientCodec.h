#pragma once

#include <functional>
#include "Session.h"
#include "log.h"
#include "net_define.h"

class ClientCodec {
   public:
    typedef std::function<void(const SessionPtr&, int op, int id, const std::string& body)> ClientMessageCallback;

    explicit ClientCodec(const ClientMessageCallback& cb) : messageCallback_(cb) {}

    void onMessage(const SessionPtr conn, BufferPtr buf) {
        while (buf->readableBytes() >= kHeaderLen)
        {
            const size_t len = buf->peekInt32();
            if (len > 65536) {
                LOG_ERROR("Invalid length");
                conn->close();
                break;
            } else if (buf->readableBytes() >= len) {
                buf->retrieve(kHeaderLen + 4);
                int op = buf->peekInt32();
                buf->retrieve(4);
                int id = buf->peekInt32();
                buf->retrieve(4);
                std::string body(buf->peek(), len - 16);
                messageCallback_(conn, op, id, body);
                buf->retrieve(len - 16);
            } else {
                break;
            }
        }
    }

    void send(const SessionPtr conn, int op, int id, const std::string& body) {
        LOG_INFO("send messageid {}",id);
        auto buf = std::make_shared<Buffer>();
        fillEmptyBuffer(buf, op, id, body);
        conn->send(buf);
    }

   private:
    void fillEmptyBuffer(BufferPtr buf, int op, int id, const std::string& body) {
        buf->appendInt16(16); //headerlen
        buf->appendInt16(1); //version
        buf->appendInt32(op);
        buf->appendInt32(id);
        buf->append(body.c_str(),body.size());
        int32_t len = sockets::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()) + 4);
        buf->prepend(&len, sizeof len);
    }

   private:
    ClientMessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};