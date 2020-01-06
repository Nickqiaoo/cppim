#include "LogicServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "util.hpp"

LogicServer::LogicServer(int thrnum, const std::string& httpip, int httpport, const std::string& rpcip, int rpcport)
    : httpserver_(thrnum, httpip, httpport), rpcserver_(thrnum, rpcip, rpcport) {}
LogicServer::~LogicServer() {}

void LogicServer::Start() {
    httpserver_.start();
    rpcserver_.registerService(&logicservice_);
    rpcserver_.start();
}

void LogicServer::PushMsgByKeysHandler(const HttpRequest& request, HttpResponsePtr response) {
    std::string query = request.query();
    std::vector<std::string> parmlist;
    common::split(query, "&", parmlist);
    std::vector<std::string> keys;
    int operation = 0;
    for (auto it : parmlist) {
        auto pos = it.find('=');
        if (pos != std::string::npos) {
            std::string op = it.substr(0, pos);
            std::string parm = it.substr(pos + 1);
            if (op == "mids") {
                common::split(parm, ",", keys);
            } else if (op == "operation") {
                operation = stoi(parm);
            }
        }
    }
    if (!keys.empty() && operation != 0) {
        response->delay();
        PushMsgByKeys(keys, operation, request.body());
    }else{
        
    }
}

void LogicServer::PushMsgByKeys(const vector<std::string>& keys, int op, const string& msg) {}