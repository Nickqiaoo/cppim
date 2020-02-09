#include <hiredis/hiredis.h>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class RedisClient {
   public:
    RedisClient(const std::string& ip, const int port, const std::string& passwd = "", const int db = 0) : ip_(ip), port_(port), passwd_(passwd), db_(db) {}
    ~RedisClient() {
        if (context_) {
            redisFree(context_);
        }
    }

    bool Connect();
    std::shared_ptr<redisReply> Execute(const std::string& cmd);
    std::shared_ptr<redisReply> Execute(const std::vector<std::string>& cmd);
    void PipeLine(const std::vector<std::string>& cmd);
    std::vector<std::string> MGet(const std::vector<std::string>& cmd);
    std::unordered_map<std::string, std::string> HGetAll(const std::string& cmd);

   private:
    bool Auth();
    bool Ping();
    bool Select();
    void Reset();
    bool Active();

   private:
    std::mutex mutex_;
    redisContext* context_{nullptr};
    std::string ip_;
    int port_{0};
    std::string passwd_;
    int db_{0};
};