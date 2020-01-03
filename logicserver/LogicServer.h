#pragma once

#include "RpcServer.h"
#include "HttpServer.h"

class LogicServer {
   public:
    LogicServer();
    ~LogicServer();

   private:
    HttpServer httpserver_;
    RpcServer rpcserver_;

};