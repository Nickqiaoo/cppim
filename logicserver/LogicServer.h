#pragma once

#include "HttpServer.h"
#include "KafkaProducer.h"
#include "LogicServiceImpl.h"
#include "RpcServer.h"

class LogicServer {
   public:
    LogicServer(int thrnum, const std::string& httpip, int httpport, const std::string& rpcip, int rpcport, const std::string& redisip, int redisport,const std::string& brokers,
                const std::string& topic);
    ~LogicServer();
    void Start();
    void PushMsgByKeysHandler(const HttpRequest& request, HttpResponsePtr response);
    void PushMsgByRoomHandler(const HttpRequest& request, HttpResponsePtr response);
    void PushMsgToAllHandler(const HttpRequest& request, HttpResponsePtr response);
    std::string generateKey();
    
   private:
    int64_t getMilliSecond();
    void PushMsgByKeys(const vector<std::string>& keys, int op, const string& msg);
    void PushMsgByRoom(const std::string& room, int op, const std::string& msg);
    void PushMsgToAll(int speed, int op, const std::string& msg);
    void PushMsg(const vector<std::string>& keys, int op, const std::string& server, const string& msg);

   private:
    int serverid_;
    HttpServer httpserver_;
    RpcServer rpcserver_;

    KafkaProducer kafkaproducer_;
    LogicServiceImpl logicservice_;
};