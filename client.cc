#include<iostream>
#include<asio.hpp>
#include "Buffer.h"

using namespace std;

int main(){
    asio::io_service service;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"),8080);
    asio::ip::tcp::socket socket(service);

    socket.connect(ep);
    Buffer writebuf;
    writebuf.append("GET /hello HTTP/1.1\r\n\r\n",23);
    size_t size = socket.write_some(asio::buffer(writebuf.peek(),writebuf.readableBytes()));
    writebuf.retrieve(size);
    
    Buffer readbuf;
    size = socket.read_some(asio::buffer(readbuf.beginWrite(),readbuf.writableBytes()));
    readbuf.hasWritten(size);
    string str(readbuf.peek(),readbuf.readableBytes());
    readbuf.retrieve(size);
    cout<<str<<endl;

    writebuf.append("GET /hello HTTP/1.1\r\n\r\n",23);
    size = socket.write_some(asio::buffer(writebuf.peek(),writebuf.readableBytes()));
    writebuf.retrieve(size);
    size = socket.read_some(asio::buffer(readbuf.beginWrite(),readbuf.writableBytes()));
    readbuf.hasWritten(size);
    str = string(readbuf.peek(),readbuf.readableBytes());

    return 0;
}