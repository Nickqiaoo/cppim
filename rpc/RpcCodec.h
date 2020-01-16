#pragma once

#include <google/protobuf/message.h>
#include <functional>

#include "net_define.h"
#include "Session.h"
#include "log.h"


namespace common {

//  int32_t len
//  uint64_t id   the highest bit: 0 request, 1 response
//  int32_t namelen
//  char    service:method[namelen]
//  char    protobuf[len-namelen-12]

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

class RpcCodec {
   public:
    enum ErrorCode {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef std::function<void(bool, uint64_t, const std::string&, const std::string&, const SessionPtr&, const char*, int32_t)> ProtobufMessageCallback;

    typedef std::function<void(const SessionPtr, Buffer*, ErrorCode)> ErrorCallback;

    explicit RpcCodec(const ProtobufMessageCallback& messageCb) : messageCallback_(messageCb), errorCallback_(defaultErrorCallback) {}

    RpcCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb) : messageCallback_(messageCb), errorCallback_(errorCb) {}

    void onMessage(const SessionPtr conn, Buffer* buf);

    void send(uint64_t id, const std::string& name, const SessionPtr& conn, const google::protobuf::Message& message) {
        // FIXME: serialize to TcpConnection::outputBuffer()
        //Buffer buf;
        //LOG_INFO("send messageid {}", id);
        auto buf = std::make_shared<Buffer>();
        fillEmptyBuffer(buf, message, id, name);
        conn->send(buf);
    }

    static const std::string& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(BufferPtr buf, const google::protobuf::Message& message, uint64_t id, const std::string& name);
    static google::protobuf::Message* createMessage(const std::string& type_name);
    static MessagePtr parse(const char* buf, int len, ErrorCode* errorCode);

   private:
    static void defaultErrorCallback(const SessionPtr, Buffer*, ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;
    const static int kHeaderLen = sizeof(int32_t);
    const static int kIdLen = sizeof(uint64_t);
    const static int kMinMessageLen = kIdLen + kHeaderLen + 3; // IdLen + nameLen + typeName
    const static int kMaxMessageLen = 64*1024*1024; 
};

};  // namespace common