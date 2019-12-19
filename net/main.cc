#include "TcpServer.h"

void *p;
int main() {
    string ip = "127.0.0.1";
    TcpServer server(5, ip, 8080);
    server.start();
    while (1) {
        sleep(100);
    }
    return 0;
}