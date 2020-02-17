#pragma once

#include "HttpResponseContext.h"
#include "TcpClient.h"

class HttpClient {
    typedef std::function<void(const HttpResponse&)> HttpResponseCallback;

   public:
    HttpClient(Loop* loop, const std::string& ip, int port) : client_(loop, ip, port) {}
    ~HttpClient() {}

    void Do(const HttpRequest& request, HttpResponseCallback cb) {
        auto buf = std::make_shared<Buffer>();
        request.appendToBuffer(buf);
        client_.send(buf);
        httpresponsecb_ = cb;
    }

    void HandleResponse(const SessionPtr& session, Buffer* buf) {
        if (!httpcontext_.parseRequest(buf)) {
            return ;
        }

        if (httpcontext_.gotAll()) {
            httpresponsecb_();
            httpcontext_.reset();
        }
    }

   private:
    HttpResponseCallback httpresponsecb_;
    HttpResponseContext httpcontext_;
    TcpClient client_;
};