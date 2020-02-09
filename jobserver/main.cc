#include "JobServer.h"
#include "cpptoml.h"
#include "log.h"
#include "gperftools/malloc_extension.h"

#ifdef GPERFTOOLS
#include <gperftools/profiler.h>
#endif

static bool terminate = false; 

static void onsignal(int s){
    ::terminate = true;
}

int main() {
    auto config = cpptoml::parse_file("config/jobserver.toml");
    auto logconfig = config->get_table("log");
    auto kafkaconfig = config->get_table("kafka");
    auto clientconfig = config->get_table("rpcclient");

    common::Log::Instance().Init(*logconfig->get_as<std::string>("name"), *logconfig->get_as<std::string>("path"),
                                 *logconfig->get_as<std::string>("level"), *logconfig->get_as<std::string>("flushlevel"),
                                 *logconfig->get_as<bool>("stdout"), *logconfig->get_as<int>("thread"));
    signal(SIGINT,onsignal);

    #ifdef GPERFTOOLS
    ProfilerStart("job.prof");
    #endif

    JobServer server(*kafkaconfig->get_as<std::string>("brokers"), *kafkaconfig->get_as<std::string>("topic"),
                     *clientconfig->get_as<std::string>("addr"), *clientconfig->get_as<int>("port"));
    server.Start();

    while (!::terminate) {
        MallocExtension::instance()->ReleaseFreeMemory();
        server.StartConsum();
    }

    #ifdef GPERFTOOLS
        ProfilerStop();
    #endif

    server.Stop();
    
    return 0;
}