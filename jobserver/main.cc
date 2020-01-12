#include "JobServer.h"
#include "cpptoml.h"
#include "log.h"

#ifdef GPERFTOOLS
#include <gperftools/profiler.h>
#endif

static void onsignal(int s){
    ProfilerStop();
}

int main() {
    auto config = cpptoml::parse_file("config/jobserver.toml");
    auto logconfig = config->get_table("log");
    auto kafkaconfig = config->get_table("kafka");
    auto clientconfig = config->get_table("rpcclient");

    common::Log::Instance().Init(*logconfig->get_as<std::string>("name"), *logconfig->get_as<std::string>("path"),
                                 *logconfig->get_as<std::string>("level"), *logconfig->get_as<std::string>("flushlevel"),
                                 *logconfig->get_as<bool>("stdout"), *logconfig->get_as<int>("thread"));
    #ifdef GPERFTOOLS
    signal(SIGINT,onsignal);
    ProfilerStart("job.prof");
    #endif
    auto loop = std::make_shared<Loop>();
    JobServer server(loop, *kafkaconfig->get_as<std::string>("brokers"), *kafkaconfig->get_as<std::string>("topic"),
                     *clientconfig->get_as<std::string>("addr"), *clientconfig->get_as<int>("port"));
    server.Start();
    loop->start();
    server.StartConsum();
    return 0;
}