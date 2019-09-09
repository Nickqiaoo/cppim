#include "HttpSession.h"

HttpSession::HttpSession(asio::io_service &io_service) : socket_(io_service) {}

void HttpSession::Start(){
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(readbuf_.beginWrite(),readbuf_.writableBytes()),[self,this](const asio::error_code& err , size_t size){
        if(!err){
            readbuf_.hasWritten(size);
            if(!context_.parseRequest(&readbuf_)){
                //send();
            }
            if(context_.gotAll()){
                //HttpCallback();

            }
            Start();
        }
    });
}

void HttpSession::Send(Buffer* buf){
    auto self(shared_from_this());
    socket_.async_write_some(asio::buffer(buf->peek(),buf->readableBytes()),[self,this](const asio::error_code& err, size_t size){
        if(!err){
            
        }
    });
}

