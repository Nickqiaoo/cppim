#include "Session.h"
#include <iostream>
#include "Loop.h"

Session::Session(LoopPtr loop, uint64_t id)
    : loop_(loop), id_(id), socket_(loop->ios()) {}
Session::~Session() {}

void Session::start() {
  socket_.non_blocking(true);
  asio::ip::tcp::no_delay nodelay(true);
  socket_.set_option(nodelay);
  read();
  send();
}

void Session::read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      asio::buffer(readbuf_.beginWrite(), readbuf_.writableBytes()),
      [self, this](const asio::error_code &err, size_t size) {
        if (!err) {
          std::cout << "recieve data" << std::endl;
          readbuf_.hasWritten(size);
          //messagecallback_(self, &readbuf_);
          writebuf_.append(readbuf_.peek(),readbuf_.readableBytes());
          readbuf_.retrieve(readbuf_.readableBytes());
          read();
        } else {
          std::cout << "read errer :" << err.message() << std::endl;
          return;
        }
      });
}

void Session::send() {
  auto self(shared_from_this());
  socket_.async_write_some(
      asio::buffer(writebuf_.peek(), writebuf_.readableBytes()),
      [self, this](const asio::error_code &err, size_t size) {
        if (!err) {
          writebuf_.retrieve(writebuf_.readableBytes());
          send();
        } else {
          std::cout << "write errer :" << err.message() << std::endl;
          return;
        }
      });
}