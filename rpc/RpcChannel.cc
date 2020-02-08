#include "RpcChannel.h"
#include "log.h"
#include <functional>

#include <google/protobuf/descriptor.h>

using namespace std::placeholders;

RpcChannel::RpcChannel() : codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3, _4, _5, _6, _7)), services_(NULL) {}

RpcChannel::RpcChannel(const SessionPtr& conn)
    : codec_(std::bind(&RpcChannel::onRpcMessage, this, _1, _2, _3, _4, _5, _6, _7)), conn_(conn), services_(NULL) {}

RpcChannel::~RpcChannel() {
    for (const auto& outstanding : outstandings_) {
        OutstandingCall out = outstanding.second;
        delete out.response;
        delete out.done;
    }
}

// Call the given method of the remote service.  The signature of this
// procedure looks the same as Service::CallMethod(), but the requirements
// are less strict in one important way:  the request and response objects
// need not be of any specific class as long as their descriptors are
// method->input_type() and method->output_type().
void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request, ::google::protobuf::Message* response, ::google::protobuf::Closure* done) {
    //LOG_INFO("callmethod");
    uint64_t id = id_.fetch_add(1);
    std::string name = method->service()->full_name() + ':' + method->name();

    OutstandingCall out = {response, done};
    {
        std::lock_guard<std::mutex> lock(mutex_);
        outstandings_[id] = out;
    }
    codec_.send(id, name, conn_, *request);
}

void RpcChannel::onMessage(const SessionPtr conn, Buffer* buf) { codec_.onMessage(conn, buf); }

void RpcChannel::onRpcMessage(bool isresponse, uint64_t id, const std::string& servicename, const std::string& methodname, const SessionPtr& conn,
                              const char* data, int32_t datalen) {
    //LOG_INFO("onRpcMessage id:{} service:{} method:{}",id,servicename,methodname);
    assert(conn == conn_);
    // printf("%s\n", message.DebugString().c_str());
    if (isresponse) {
        OutstandingCall out = {NULL, NULL};

        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::map<uint64_t, OutstandingCall>::iterator it = outstandings_.find(id);
            if (it != outstandings_.end()) {
                out = it->second;
                outstandings_.erase(it);
            }
        }

        if (out.response) {
            std::unique_ptr<google::protobuf::Message> d(out.response);
            if (out.response->ParseFromArray(data, datalen)) {
                if (out.done) {
                    out.done->Run();
                }
            }
        }
    } else {  // request
        ErrorCode error = WRONG_PROTO;
        if (services_) {
            std::map<std::string, google::protobuf::Service*>::const_iterator it = services_->find(servicename);
            if (it != services_->end()) {
                google::protobuf::Service* service = it->second;
                assert(service != NULL);
                const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
                const google::protobuf::MethodDescriptor* method = desc->FindMethodByName(methodname);
                if (method) {
                    std::unique_ptr<google::protobuf::Message> request(service->GetRequestPrototype(method).New());
                    if (request->ParseFromArray(data, datalen)) {
                        google::protobuf::Message* response = service->GetResponsePrototype(method).New();
                        // response is deleted in doneCallback
                        // TODO modify closure
                        std::string name = servicename + ":" + methodname + ":" + std::to_string(id);
                        service->CallMethod(method, NULL, request.get(), response, ::google::protobuf::internal::NewCallback(this, &RpcChannel::doneCallback, response, name));
                        error = NO_ERROR;
                    }else{
                        error = PARSE_ERROR;
                    }
                } else {
                    error = NO_METHOD;
                }
            } else {
                error = NO_SERVICE;
            }
        } else {
            error = NO_SERVICE;
        }
        if (error != NO_ERROR) {
            // RpcMessage response;
            // response.set_type(RESPONSE);
            // response.set_id(message.id());
            // response.set_error(error);
            // codec_.send(conn_, response);
        }
    }
}

void RpcChannel::doneCallback(::google::protobuf::Message* response, string name) {
    std::unique_ptr<google::protobuf::Message> d(response);
    auto pos = name.find_last_of(':');
    if (pos != std::string::npos) {
        uint64_t id = std::strtoull(name.substr(pos + 1).c_str(), nullptr, 0);
        name = name.substr(0, pos);
        id = id | 0x8000000000000000;
        codec_.send(id, name, conn_, *response);
    }
}
