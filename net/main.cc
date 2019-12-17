#include "TcpServer.h"

int main() {
    string ip = "127.0.0.1";
    TcpServer server(5, ip, 8080);
    server.start();
    sleep(100);
    return 0;
}