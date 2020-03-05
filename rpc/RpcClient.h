#include "TcpClient.h"
#include "RpcChannel.h"
#include "net_define.h"

using namespace std::placeholders;

class RpcClient {
   public:
    RpcClient(LoopPtr loop, const string& ip, int port) : client_(loop, ip, port), channel_(new RpcChannel), stub_(channel_.get()) {
        client_.setConnectionCallback(std::bind(&RpcClient::onConnection, this, _1));
        client_.setMessageCallback(std::bind(&RpcChannel::onMessage, channel_.get(), _1, _2));
    }
    ~RpcClient() {}

    void connect() { client_.connect(); }
    void onConnection(const SessionPtr& conn) {
        LOG_INFO("client onConnection");
        channel_->setConnection(conn);
    }

    void Echo(echo::EchoRequest* request){
        echo::EchoResponse* response = new echo::EchoResponse;
        stub_.Echo(NULL, request, response, NewCallback(this,&RpcClient::echoDoneCallback, response));
    }

    void echoDoneCallback(echo::EchoResponse* response){

    }

   private:
    TcpClient client_;
    RpcChannelPtr channel_;
    echo::EchoService_Stub stub_;
};