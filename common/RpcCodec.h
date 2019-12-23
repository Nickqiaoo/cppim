#pragma once;

#include <google/protobuf/message.h>
#include <functional>

#include "net_define.h"
#include "Session.h"


namespace common {

//  int32_t len
//  uint64_t id   the highest bit: 0 request, 1 response
//  int32_t namelen
//  char    service:method[namelen]
//  char    protobuf[len-namelen-16]

typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

//
// FIXME: merge with RpcCodec
//
class ProtobufCodec {
   public:
    enum ErrorCode {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef std::function<void(const SessionPtr&, const MessagePtr&)> ProtobufMessageCallback;

    typedef std::function<void(const SessionPtr&, BufferPtr, ErrorCode)> ErrorCallback;

    explicit ProtobufCodec(const ProtobufMessageCallback& messageCb) : messageCallback_(messageCb), errorCallback_(defaultErrorCallback) {}

    ProtobufCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb) : messageCallback_(messageCb), errorCallback_(errorCb) {}

    void onMessage(const SessionPtr& conn, BufferPtr buf);

    void send(const SessionPtr& conn, const google::protobuf::Message& message) {
        // FIXME: serialize to TcpConnection::outputBuffer()
        //Buffer buf;
        auto buf = std::make_shared<Buffer>();
        fillEmptyBuffer(buf, message);
        conn->send(buf);
    }

    static const std::string& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(BufferPtr buf, const google::protobuf::Message& message);
    static google::protobuf::Message* createMessage(const std::string& type_name);
    static MessagePtr parse(const char* buf, int len, ErrorCode* errorCode);

   private:
    static void defaultErrorCallback(const SessionPtr&, BufferPtr, ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;

    const static int kHeaderLen = sizeof(int32_t);
    const static int kIdLen = sizeof(uint64_t);
    const static int kMinMessageLen = kIdLen + kHeaderLen + 3; // IdLen + nameLen + typeName
    const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimi
};

};  // namespace common