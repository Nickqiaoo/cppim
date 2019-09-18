#include <iostream>
#include "HttpSession.h"

HttpSession::HttpSession(asio::io_service &io_service) : socket_(io_service) {}
HttpSession::~HttpSession(){}

void HttpSession::Start(){
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(readbuf_.beginWrite(),readbuf_.writableBytes()),[self,this](const asio::error_code& err , size_t size){
        if(!err){
            std::cout<<"recieve data"<<std::endl;
            readbuf_.hasWritten(size);
            if(!context_.parseRequest(&readbuf_)){
                Buffer buf;
                buf.append("HTTP/1.1 400 Bad Request\r\n\r\n",28);
                Send(&buf);
            }
            if(context_.gotAll()){
                //HttpCallback();
                context_.reset();
                Buffer buf;
                buf.append("HTTP/1.1 200 OK\r\n\r\n",19);
                Send(&buf);
            }
            Start();
        }else{
            std::cout<<"read errer :"<<err.message()<<std::endl;
            return ;
        }
    });
}

void HttpSession::Send(Buffer* buf){
    auto self(shared_from_this());
    async_write(socket_,asio::buffer(buf->peek(),buf->readableBytes()),[self,this](const asio::error_code& err, size_t size){
        if(!err){
            std::cout<<"send data :"<<size<<std::endl;
        }else{
            std::cout<<"send errer :"<<err.message()<<std::endl;
            return ;
        }
    });
}

