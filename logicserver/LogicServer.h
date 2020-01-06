#pragma once

#include "RpcServer.h"
#include "HttpServer.h"
#include "LogicServiceImpl.h"

class LogicServer {
   public:
    LogicServer(int thrnum, const std::string& httpip, int httpport, const std::string& rpcip, int rpcport);
    ~LogicServer();
    void Start();
    void PushMsgByKeysHandler(const HttpRequest& request, HttpResponsePtr response);

   private:
    void PushMsgByKeys(const vector<std::string>& keys, int op, const string& msg);
    
   private:
    HttpServer httpserver_;
    RpcServer rpcserver_;

    LogicServiceImpl logicservice_;
};