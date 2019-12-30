#include "Loop.h"
#include "RpcChannel.h"
#include "Tcpserver.h"
#include "echo.pb.h"
#include "log.h"

#include <stdio.h>
#include <unistd.h>
#include <functional>

using namespace std::placeholders;

class RpcClient {
   public:
    RpcClient(LoopPtr loop) : session_(std::make_shared<Session>(loop, 1)), channel_(new RpcChannel), stub_(channel_.get()) {
        session_->setConnectionCallback(std::bind(&RpcClient::onConnection, this, _1));
        session_->setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2));
        // client_.enableRetry();
    }

    void connect() { session_->connect("127.0.0.1", 8080); }

   private:
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        // channel_.reset(new RpcChannel(conn));

        for (int i = 0; i < 1000; i++) {
            channel_->setConnection(conn);
            echo::EchoRequest request;
            request.set_msg("hello");
            echo::EchoResponse* response = new echo::EchoResponse;
            stub_.Echo(NULL, &request, response, NewCallback(this, &RpcClient::echo, response));
        }
    }

    void echo(echo::EchoResponse* resp) {
        echo::EchoRequest request;
        request.set_msg("hello");
        echo::EchoResponse* response = new echo::EchoResponse;
        stub_.Echo(NULL, &request, response, NewCallback(this, &RpcClient::echo, response));
    }

    SessionPtr session_;
    RpcChannelPtr channel_;
    echo::EchoService::Stub stub_;
};

typedef std::shared_ptr<RpcClient> RpcClientPtr;

int main(int argc, char* argv[]) {
    common::Log::Instance().Init("tcpclient", "./clientlog", "trace", "debug", true, 1);
    auto loop = std::make_shared<Loop>();
    std::vector<RpcClientPtr> clientvec;
    for (int i = 0; i < 100; i++) {
        clientvec.emplace_back(std::make_shared<RpcClient>(loop));
        clientvec[i]->connect();
    }
    loop->start();
    while (1) {
        sleep(100);
    }
    return 0;
}