#include "RpcCodec.h"
#include "endian.h"

namespace common {

void RpcCodec::fillEmptyBuffer(BufferPtr buf, const google::protobuf::Message& message, uint64_t id, const std::string& name) {
    // buf->retrieveAll();
    assert(buf->readableBytes() == 0);

    buf->appendInt64(id);
    int32_t nameLen = static_cast<int32_t>(name.size() + 1);
    buf->appendInt32(nameLen);
    buf->append(name.c_str(), nameLen);

    int byte_size = message.ByteSize();
    buf->ensureWritableBytes(byte_size);

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->beginWrite());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);
    buf->hasWritten(byte_size);

    int32_t len = sockets::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
    buf->prepend(&len, sizeof len);
}

namespace {
const string kNoErrorStr = "NoError";
const string kInvalidLengthStr = "InvalidLength";
const string kCheckSumErrorStr = "CheckSumError";
const string kInvalidNameLenStr = "InvalidNameLen";
const string kUnknownMessageTypeStr = "UnknownMessageType";
const string kParseErrorStr = "ParseError";
const string kUnknownErrorStr = "UnknownError";
}  // namespace

const string& RpcCodec::errorCodeToString(ErrorCode errorCode) {
    switch (errorCode) {
        case kNoError:
            return kNoErrorStr;
        case kInvalidLength:
            return kInvalidLengthStr;
        case kCheckSumError:
            return kCheckSumErrorStr;
        case kInvalidNameLen:
            return kInvalidNameLenStr;
        case kUnknownMessageType:
            return kUnknownMessageTypeStr;
        case kParseError:
            return kParseErrorStr;
        default:
            return kUnknownErrorStr;
    }
}

void RpcCodec::defaultErrorCallback(const SessionPtr& conn, BufferPtr buf, ErrorCode errorCode) {}

int32_t asInt32(const char* buf) {
    int32_t be32 = 0;
    ::memcpy(&be32, buf, sizeof(be32));
    return sockets::networkToHost32(be32);
}

uint64_t asUInt64(const char* buf) {
    uint64_t be64 = 0;
    ::memcpy(&be64, buf, sizeof(be64));
    return sockets::networkToHost64(be64);
}

void RpcCodec::onMessage(const SessionPtr& conn, BufferPtr buf) {
    while (buf->readableBytes() >= kMinMessageLen + kHeaderLen) {
        const int32_t len = buf->peekInt32();
        if (len > kMaxMessageLen || len < kMinMessageLen) {
            errorCallback_(conn, buf, kInvalidLength);
            break;
        } else if (buf->readableBytes() >= static_cast<size_t>(len + kHeaderLen)) {
            MessagePtr message;
            ErrorCode errorCode = kNoError;
            const char* idstart = buf->peek() + kHeaderLen;
            uint64_t id = asUInt64(idstart);
            bool isresponse = id >> 63;
            id = id & 0x7fffffffffffffff;
            int32_t nameLen = asInt32(idstart + kIdLen);
            std::string service, method;
            if (nameLen >= 3 && nameLen <= len - 2 * kHeaderLen) {
                std::string typeName(idstart + kHeaderLen + kIdLen, idstart + kHeaderLen + kIdLen + nameLen + 1);
                auto pos = typeName.find(':');
                if (pos != std::string::npos) {
                    service = typeName.substr(0, pos);
                    method = typeName.substr(pos);
                    // create message object
                    message.reset(createMessage(typeName));
                    if (message) {
                        // parse from buffer
                        const char* data = idstart + kHeaderLen + kIdLen + nameLen;
                        int32_t dataLen = len - nameLen - 2 * kHeaderLen - kIdLen;
                        if (message->ParseFromArray(data, dataLen)) {
                            errorCode = kNoError;
                        } else {
                            errorCode = kParseError;
                        }
                    } else {
                        errorCode = kUnknownMessageType;
                    }
                } else {
                    errorCode = kInvalidNameLen;
                }
            } else {
                errorCode = kInvalidNameLen;
            }

            if (errorCode == kNoError && message) {
                messageCallback_(isresponse, id, service, method, conn, message);
                buf->retrieve(kHeaderLen + len);
            } else {
                errorCallback_(conn, buf, errorCode);
                break;
            }
        } else {
            break;
        }
    }
}

/*
google::protobuf::Message* RpcCodec::createMessage(const std::string& typeName) {
    google::protobuf::Message* message = NULL;
    const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (descriptor) {
        const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}

MessagePtr RpcCodec::parse(const char* buf, int len, ErrorCode* error) {
    MessagePtr message;

    // get message type name
    uint64_t id = asUInt64(buf);
    int32_t nameLen = asInt32(buf + 8);
    if (nameLen >= 2 && nameLen <= len - 2 * kHeaderLen) {
        std::string typeName(buf + kHeaderLen + kIdLen, buf + kHeaderLen + kIdLen + nameLen - 1);
        // create message object
        message.reset(createMessage(typeName));
        if (message) {
            // parse from buffer
            const char* data = buf + kHeaderLen + kIdLen + nameLen;
            int32_t dataLen = len - nameLen - 2 * kHeaderLen - kIdLen;
            if (message->ParseFromArray(data, dataLen)) {
                *error = kNoError;
            } else {
                *error = kParseError;
            }
        } else {
            *error = kUnknownMessageType;
        }
    } else {
        *error = kInvalidNameLen;
    }

    return message;
}
*/

}  // namespace common