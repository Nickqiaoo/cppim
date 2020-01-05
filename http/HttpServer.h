#pragma once

#include <unordered_map>
#include "TcpServer.h"

class HttpRequest;
class HttpResponse;

class HttpServer {
  typedef std::function<void(const SessionPtr,const HttpRequest&,HttpResponse*)> HttpCallback;

  public:
    HttpServer(int thrnum, const std::string& ip, int port);
    ~HttpServer();
    void Start();
    private:
    void onConnection(const SessionPtr& conn);
    void onMessage(const SessionPtr, const BufferPtr);
    void onRequest(const SessionPtr& conn, const HttpRequest& req); 
    
  private:
   TcpServer tcpserver_;
   std::unordered_map<std::string,HttpCallback> handler_;
};