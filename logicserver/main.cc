#include "LogicServer.h"
#include "cpptoml.h"
#include "log.h"
#include "gperftools/malloc_extension.h"

#ifdef GPERFTOOLS
#include <gperftools/profiler.h>
#endif

static bool terminate = false; 

static void onsignal(int s) {
    ::terminate = true;
}

int main() {
    auto config = cpptoml::parse_file("config/logicserver.toml");
    auto logconfig = config->get_table("log");
    auto httpconfig = config->get_table("httpserver");
    auto rpcconfig = config->get_table("rpcserver");
    auto redisconfig = config->get_table("redis");
    auto logicconfig = config->get_table("logicserver");
    auto kafkaconfig = config->get_table("kafka");

    common::Log::Instance().Init(*logconfig->get_as<std::string>("name"), *logconfig->get_as<std::string>("path"),
                                 *logconfig->get_as<std::string>("level"), *logconfig->get_as<std::string>("flushlevel"),
                                 *logconfig->get_as<bool>("stdout"), *logconfig->get_as<int>("thread"));
    signal(SIGINT, onsignal);

    #ifdef GPERFTOOLS
        ProfilerStart("logic.prof");
    #endif
    
    LogicServer server(*logicconfig->get_as<int>("netthr"), *httpconfig->get_as<std::string>("addr"), *httpconfig->get_as<int>("port"),
                       *rpcconfig->get_as<std::string>("addr"), *rpcconfig->get_as<int>("port"), *redisconfig->get_as<std::string>("addr"),
                       *redisconfig->get_as<int>("port"), *kafkaconfig->get_as<std::string>("brokers"), *kafkaconfig->get_as<std::string>("topic"),
                       *logicconfig->get_as<int>("serverid"));
    server.Start();

    while (!::terminate) {
        sleep(30);
        MallocExtension::instance()->ReleaseFreeMemory();
    }

    server.Stop();

    #ifdef GPERFTOOLS
        ProfilerStop();
    #endif

    return 0;
}