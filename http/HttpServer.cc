#include "HttpServer.h"
#include <iostream>
#include "HttpResponse.h"
#include "HttpSession.h"
#include "log.h"

using namespace std::placeholders;

HttpServer::HttpServer(int thrnum, const std::string& ip, int port) : tcpserver_(thrnum, ip, port) {
    tcpserver_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2));
    std::srand(time(nullptr));
}

HttpServer::~HttpServer() {}

void HttpServer::start() {
    tcpserver_.setNewSessionCallback<HttpSession>();
    tcpserver_.start();
}

void HttpServer::stop() { tcpserver_.stop(); }

void HttpServer::doResponse(HttpResponsePtr response) {
    auto buf = std::make_shared<Buffer>();
    response->appendToBuffer(buf);
    auto session = response->getSessionPtr();
    session->send(buf);
}

void HttpServer::onMessage(const SessionPtr conn, Buffer* buf) {
    auto httpsession = static_pointer_cast<HttpSession>(conn);
    HttpContext* context = httpsession->getContext();

    if (!context->parseRequest(buf)) {
        auto buf = std::make_shared<Buffer>();
        buf->append("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->send(buf);
    }

    if (context->gotAll()) {
        onRequest(httpsession, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const HttpSessionPtr& conn, const HttpRequest& req) {
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" || (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    auto response = std::make_shared<HttpResponse>(conn, close);
    auto it = handler_.find(req.path());
    if (it != handler_.end()) {
        auto res = limiter_.Allow();
        //res.second = true;
        auto stat = limiter_.Stat();
        std::string status = "fail";
        if(res.second){
            status = "success";
        }
        LOG_INFO("Allow:{} Limit Stat limit:{} inFlight:{} minRTT:{} lastRTT:{}",status,stat.limit_,stat.inFlight_,stat.minRTT_,stat.lastRTT_);
        if (res.second) {
            it->second(req, response);
            response->setDelay();
            if (!response->delay()) {
                response->setStatusCode(HttpResponse::k200Ok);
                response->setStatusMessage("OK");
                doResponse(response);
            }
            conn->addTimerHandler(10,[=]{
                res.first(Success);
                response->setStatusCode(HttpResponse::k200Ok);
                response->setStatusMessage("OK");
                doResponse(response);
            });
        } else {
            response->setStatusCode(HttpResponse::k200Ok);
            response->setStatusMessage("Too Frequency");
            doResponse(response);
        }
    } else {
        response->setStatusCode(HttpResponse::k404NotFound);
        response->setStatusMessage("NotFound");
        doResponse(response);
    }
    if (response->closeConnection()) {
        conn->close();
    }
}
