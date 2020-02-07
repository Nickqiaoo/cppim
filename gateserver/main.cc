#include "GateServer.h"
#include "cpptoml.h"
#include "gperftools/malloc_extension.h"
#include "log.h"

#ifdef GPERFTOOLS
#include <gperftools/profiler.h>
#endif

static bool terminate = false; 

static void onsignal(int s) {
    ::terminate = true;
}

int main() {
    auto config = cpptoml::parse_file("config/gateserver.toml");
    auto logconfig = config->get_table("log");
    auto tcpconfig = config->get_table("tcpserver");
    auto rpcconfig = config->get_table("rpcserver");
    auto gateconfig = config->get_table("gateserver");
    auto clientconfig = config->get_table("rpcclient");

    common::Log::Instance().Init(*logconfig->get_as<std::string>("name"), *logconfig->get_as<std::string>("path"), *logconfig->get_as<std::string>("level"),
                                 *logconfig->get_as<std::string>("flushlevel"), *logconfig->get_as<bool>("stdout"), *logconfig->get_as<int>("thread"));
    signal(SIGINT, onsignal);

    #ifdef GPERFTOOLS
        ProfilerStart("gate.prof");
    #endif

    auto loop = std::make_shared<Loop>();
    GateServer server(loop, *gateconfig->get_as<int>("netthr"), *tcpconfig->get_as<std::string>("addr"), *tcpconfig->get_as<int>("port"),
                      *rpcconfig->get_as<std::string>("addr"), *rpcconfig->get_as<int>("port"), *clientconfig->get_as<std::string>("addr"),
                      *clientconfig->get_as<int>("port"), *gateconfig->get_as<std::string>("serverid"));
    server.Start();
    loop->start();
    
    while (!::terminate) {
        sleep(5);
        MallocExtension::instance()->ReleaseFreeMemory();
    }

    loop->stop();
    server.Stop();

    #ifdef GPERFTOOLS
        ProfilerStop();
    #endif

    return 0;
}