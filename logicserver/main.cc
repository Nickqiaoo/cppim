#include "LogicServer.h"
#include "log.h"
#include "cpptoml.h"

#ifdef GPERFTOOLS
#include <gperftools/profiler.h>
#endif

static void onsignal(int s){
#ifdef GPERFTOOLS
    ProfilerStop();
#endif
    exit(1);
}

int main() {
    auto config = cpptoml::parse_file("config/logicserver.toml");
    auto logconfig = config->get_table("log");
    auto httpconfig = config->get_table("httpserver");
    auto rpcconfig = config->get_table("rpcserver");
    auto logicconfig = config->get_table("logicserver");
    auto kafkaconfig = config->get_table("kafka");

    common::Log::Instance().Init(*logconfig->get_as<std::string>("name"), *logconfig->get_as<std::string>("path"),
                                 *logconfig->get_as<std::string>("level"), *logconfig->get_as<std::string>("flushlevel"),
                                 *logconfig->get_as<bool>("stdout"), *logconfig->get_as<int>("thread"));
    #ifdef GPERFTOOLS
    signal(SIGINT,onsignal);
    ProfilerStart("logic.prof");
    #endif
    LogicServer server(*logicconfig->get_as<int>("netthr"), *httpconfig->get_as<std::string>("addr"), *httpconfig->get_as<int>("port"),
                      *rpcconfig->get_as<std::string>("addr"), *rpcconfig->get_as<int>("port"), *kafkaconfig->get_as<std::string>("brokers"),
                      *kafkaconfig->get_as<std::string>("topic"));
    server.Start();
    while (1) {
        sleep(100);
    }
    return 0;
}