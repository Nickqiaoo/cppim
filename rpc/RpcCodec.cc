#include "RpcCodec.h"
#include "EndianTransfer.h"
#include "log.h"

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

void RpcCodec::defaultErrorCallback(const SessionPtr conn, Buffer* buf, ErrorCode errorCode) {
    LOG_ERROR("parse message error");
}

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

void RpcCodec::onMessage(const SessionPtr conn, Buffer* buf) {
    //LOG_INFO("rpc onMessage");
    bool flag=0;
    LOG_INFO("buf size:{},num:{}",buf->readableBytes(),num);
    num++;
    if(num>10){
        exit(1);
    }
    while (buf->readableBytes() >= kMinMessageLen + kHeaderLen) {
        const int32_t len = buf->peekInt32();
        LOG_INFO("len:{}",len);
        if (len > kMaxMessageLen || len < kMinMessageLen) {
            errorCallback_(conn, buf, kInvalidLength);
            break;
        } else if (buf->readableBytes() >= static_cast<size_t>(len + kHeaderLen)) {
            ErrorCode errorCode = kNoError;
            const char* idstart = buf->peek() + kHeaderLen;
            uint64_t id = asUInt64(idstart);
            bool isresponse = id >> 63;
            id = id & 0x7fffffffffffffff;
            int32_t nameLen = asInt32(idstart + kIdLen);
            std::string service, method;
            const char* data;
            int32_t datalen;
            if (nameLen >= 3 && nameLen <= len - 2 * kHeaderLen) {
                std::string typeName(idstart + kHeaderLen + kIdLen, idstart + kHeaderLen + kIdLen + nameLen);
                auto pos = typeName.find(':');
                if (pos != std::string::npos) {
                    service = typeName.substr(0, pos);
                    method = typeName.substr(pos + 1);
                    data = idstart + kHeaderLen + kIdLen + nameLen;
                    datalen = len - nameLen - kHeaderLen - kIdLen;
                } else {
                    errorCode = kInvalidNameLen;
                }
            } else {
                errorCode = kInvalidNameLen;
            }

            if (errorCode == kNoError) {
                if(flag==0){
                    num--;
                }
                flag=1;
                messageCallback_(isresponse, id, service, method, conn, data, datalen);
                buf->retrieve(kHeaderLen + len);
            } else {
                errorCallback_(conn, buf, errorCode);
                buf->retrieve(kHeaderLen + len);
                break;
            }
        } else {
            break;
        }
    }
}


}  // namespace common