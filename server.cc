#include "HttpServer.h"
#include <asio.hpp>

int main(){
    asio::io_service ios;
    asio::io_service::work work(ios);
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"),8080);
    HttpServer server(ios,ep);
    server.Start();
    ios.run();
    return 0;
}