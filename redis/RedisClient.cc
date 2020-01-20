#include "RedisClient.h"
#include <algorithm>
#include <cstring>
#include "log.h"

bool RedisClient::Connect() {
    struct timeval tv = {2, 500000};  // 2.5secs
    context_ = redisConnectWithTimeout(ip_.c_str(), port_, tv);
    if (context_ == nullptr || context_->err) {
        Reset();
        return false;
    }

    if (!Auth() || !Ping() || !Select()) {
        return false;
    }
    return true;
}

std::shared_ptr<redisReply> RedisClient::Execute(const std::string& cmd) {
    if (!Active()) {
        return nullptr;
    }
    redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(context_, cmd.c_str()));
    return std::shared_ptr<redisReply>(reply, [](redisReply* p) { freeReplyObject(p); });
}

std::shared_ptr<redisReply> RedisClient::Execute(const std::vector<std::string>& cmd) {
    std::vector<const char*> argv;
    std::transform(cmd.begin(), cmd.end(), std::back_inserter(argv), [](const std::string& s) { return s.c_str(); });
    // Construct a size_t* of string lengths from the vector
    std::vector<size_t> argvlen;
    std::transform(cmd.begin(), cmd.end(), std::back_inserter(argvlen), [](const std::string& s) { return s.size(); });

    if (!Active()) {
        return nullptr;
    }
    redisReply* reply = reinterpret_cast<redisReply*>(redisCommandArgv(context_, argv.size(), &argv[0], &argvlen[0]));
    return std::shared_ptr<redisReply>(reply, [](redisReply* p) { freeReplyObject(p); });
}

void RedisClient::PipeLine(const std::vector<std::string>& cmd) {
    redisReply* reply;
    for (auto it : cmd) {
        redisAppendCommand(context_, it.c_str());
    }
    for (size_t i = 0; i < cmd.size(); i++) {
        redisGetReply(context_, reinterpret_cast<void**>(&reply));
        if (!reply) {
            continue;
        }
        if (REDIS_REPLY_STATUS == reply->type && (strcasecmp(reply->str, "OK") == 0)) {
        } else {
            LOG_ERROR("redis pipeline execute error:{}", reply->str);
        }
        freeReplyObject(reply);
    }
}

std::vector<std::string> RedisClient::MGet(const std::vector<std::string>& cmd) {
    std::vector<std::string> command;
    command.emplace_back("MGET");
    for (auto elem : cmd) {
        command.emplace_back(std::move(elem));
    }
    auto reply = Execute(command);
    command.clear();
    if (reply) {
        if (reply->type == REDIS_REPLY_NIL) {
        } else if (reply->type == REDIS_REPLY_ARRAY) {
            redisReply* rep;
            for (size_t i = 0; i < reply->elements; i++) {
                rep = reply->element[i];
                if (rep->type == REDIS_REPLY_STRING) {
                    command.emplace_back(rep->str, rep->len);
                } else if (rep->type == REDIS_REPLY_INTEGER) {
                    command.emplace_back(std::to_string(rep->integer));
                }
            }
        } else {
            LOG_ERROR("redis execute error:{}", reply->str);
        }
    } else {
        LOG_ERROR("redis connect error:{}");
    }
    return command;
}

std::unordered_map<std::string, std::string> RedisClient::HGetAll(const std::string& cmd) {
    std::vector<std::string> command;
    std::unordered_map<std::string, std::string> hmap;
    std::string key, value;
    command.emplace_back("HGETALL");
    command.emplace_back(std::move(cmd));
    auto reply = Execute(command);
    if (reply) {
        if (reply->type == REDIS_REPLY_NIL) {
        } else if (reply->type == REDIS_REPLY_ARRAY) {
            redisReply* rep;
            for (size_t i = 0; i < reply->elements; i += 2) {
                rep = reply->element[i];
                if (rep->type == REDIS_REPLY_STRING) {
                    key = std::string(rep->str, rep->len);
                }
                rep = reply->element[i + 1];
                if (rep->type == REDIS_REPLY_STRING) {
                    value = std::string(rep->str, rep->len);
                } else if (rep->type == REDIS_REPLY_INTEGER) {
                    value = std::to_string(rep->integer);
                }
                hmap.insert({key, value});
            }
        } else {
            LOG_ERROR("redis execute error:{}", reply->str);
        }
    } else {
        LOG_ERROR("redis connect error:{}");
    }
    return hmap;
}

bool RedisClient::Auth() {
    if (passwd_.empty()) {
        return true;
    }
    redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(context_, "AUTH %s", passwd_.c_str()));
    if (!reply) {
        return false;
    }
    if (REDIS_REPLY_STATUS == reply->type && (strcasecmp(reply->str, "OK") == 0)) {
        freeReplyObject(reply);
        return true;
    } else {
        freeReplyObject(reply);
        return false;
    }
}

bool RedisClient::Ping() {
    redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(context_, "PING"));
    if (!reply) {
        return false;
    }
    if (REDIS_REPLY_STATUS == reply->type && (strcasecmp(reply->str, "PONG") == 0)) {
        freeReplyObject(reply);
        return true;
    } else {
        freeReplyObject(reply);
        return true;
    }
}

bool RedisClient::Select() {
    redisReply* reply = reinterpret_cast<redisReply*>(redisCommand(context_, "SELECT %d", db_));
    if (!reply) {
        return false;
    }
    if (REDIS_REPLY_STATUS == reply->type && (strcasecmp(reply->str, "OK") == 0)) {
        freeReplyObject(reply);
        return true;
    } else {
        freeReplyObject(reply);
        return true;
    }
}

void RedisClient::Reset() {
    if (context_) {
        redisFree(context_);
        context_ = nullptr;
    }
}

bool RedisClient::Active() {
    if (context_ && !context_->err) {
        return true;
    }
    Reset();
    int count = 0;
    while (!context_) {
        Connect();
        if (++count > 3) {
            return false;
        }
    }
    return true;
}