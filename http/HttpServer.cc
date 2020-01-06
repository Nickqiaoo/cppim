#include "HttpServer.h"
#include <iostream>
#include "HttpResponse.h"
#include "HttpSession.h"

using namespace std::placeholders;

HttpServer::HttpServer(int thrnum, const std::string& ip, int port) : tcpserver_(thrnum, ip, port) {
    tcpserver_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2));
}

HttpServer::~HttpServer() {}

void HttpServer::Start() {
    tcpserver_.setNewHttpSessionCalback();
    tcpserver_.start();
}

void HttpServer::onMessage(const SessionPtr conn, const BufferPtr buf) {
    auto httpsession = static_pointer_cast<HttpSession>(conn);
    HttpContext* context = httpsession->getContext();

    if (!context->parseRequest(buf)) {
        auto buf = std::make_shared<Buffer>();
        buf->append("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->send(buf);
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const SessionPtr& conn, const HttpRequest& req) {
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" || (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    auto it = handler_.find(req.path());
    if (it != handler_.end()) {
        it->second(conn, req, &response);
        auto buf = std::make_shared<Buffer>();
        response.appendToBuffer(buf);
        conn->send(buf);
    } else {
    }
    if (response.closeConnection()) {
        conn->close();
    }
}