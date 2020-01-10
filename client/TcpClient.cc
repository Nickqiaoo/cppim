#include "TcpClient.h"
#include <iostream>
#include "ClientCodec.h"
#include "Loop.h"

using namespace std;
using namespace std::placeholders;

void onClientMessageCallback(const SessionPtr& session, int op, int id, const std::string& body) { cout << "onMessage" << body << endl; }

class Client {
   public:
    Client(LoopPtr loop, const std::string& ip, int port) : client_(loop, ip, port), codec_(bind(&onClientMessageCallback, _1, _2, _3, _4)) {
        client_.setMessageCallback(std::bind(&ClientCodec::onMessage, &codec_, _1, _2));
        client_.setConnectionCallback(std::bind(&Client::onConnection, this, _1));
    }
    ~Client() {}
    void connect() { client_.connect(); }
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        codec_.send(client_.getSession(), 1, 1, "test");
    }

   private:
    TcpClient client_;
    ClientCodec codec_;
};

int main() {
    common::Log::Instance().Init("tcpclient", "./clientlog", "trace", "debug", true, 1);
    auto loop = std::make_shared<Loop>();
    Client client(loop, "127.0.0.1", 8080);
    client.connect();
    loop->start();
    while (1) {
        sleep(100);
    }
    return 0;
}