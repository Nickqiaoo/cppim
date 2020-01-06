#pragma once

#include "RpcServer.h"
#include "HttpServer.h"

class LogicServer {
   public:
    LogicServer(int thrnum, const std::string& httpip, int httpport, const std::string& rpcip, int rpcport);
    ~LogicServer();
    void PushMsgByKeysHandler(const SessionPtr session, const HttpRequest& request, HttpResponse* response);

   private:
    void PushMsgByKeys(const vector<std::string>& keys,const string& msg);
    
   private:
    HttpServer httpserver_;
    RpcServer rpcserver_;

};