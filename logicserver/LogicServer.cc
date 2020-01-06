#include "LogicServer.h"
#include "HttpRequest.h"

LogicServer::LogicServer(int thrnum, const std::string& httpip, int httpport, const std::string& rpcip, int rpcport)
    : httpserver_(thrnum, httpip, httpport), rpcserver_(thrnum, rpcip, rpcport) {}
LogicServer::~LogicServer() {}

void LogicServer::PushMsgByKeysHandler(const SessionPtr session, const HttpRequest& request, HttpResponse* response) {
    std::string query = request.query();
}

void LogicServer::PushMsgByKeys(const vector<std::string>& keys, const string& msg) {}