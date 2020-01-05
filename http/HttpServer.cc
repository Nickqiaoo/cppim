#include "HttpServer.h"
#include "HttpSession.h"
#include "HttpResponse.h"
#include <iostream>

HttpServer::HttpServer(int thrnum, const std::string& ip, int port) : tcpserver_(thrnum, ip, port) {}

HttpServer::~HttpServer() {}

void HttpServer::Start() { tcpserver_.start(); }

void HttpServer::onMessage(const SessionPtr conn, const BufferPtr buf) {
    auto httpsession = static_pointer_cast<HttpSession>(conn);
    HttpContext* context = httpsession->getContext();

    if (!context->parseRequest(buf)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
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
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection()) {
        conn->shutdown();
    }
}