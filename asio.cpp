#include<iostream>
#include<asio.hpp>


using namespace asio;

io_service service;
ip::tcp::endpoint ep (ip::tcp::v4(),8080);
ip::tcp::acceptor acc(service, ep);
char buf[1024];

void doRead(std::shared_ptr<ip::tcp::socket> sock);

void doWrite(std::shared_ptr<ip::tcp::socket> sock){
    sock->async_write_some(buffer(buf),[sock](const asio::error_code &err, size_t nwrite){
        doRead(sock);
    });
}
 
void doRead(std::shared_ptr<ip::tcp::socket> sock){
    sock->async_read_some(buffer(buf),[sock](const asio::error_code &err, size_t nread){
        std::cout<<buf;
        doWrite(sock);
    });
}

void start_accept(std::shared_ptr<ip::tcp::socket> sock){
    acc.async_accept(*sock, [sock](const asio::error_code &err){
        doRead(sock);
        auto sock = std::make_shared<ip::tcp::socket>(service);
        start_accept(sock);
    });
}

int main(){
    auto sock = std::make_shared<ip::tcp::socket>(service);
    start_accept(sock);
    service.post([]{std::cout<<"hello"<<std::endl;});
    service.run();
    return 0;
}
