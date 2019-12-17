#include "Session.h"
#include <iostream>
#include "Loop.h"

Session::Session(LoopPtr loop, uint64_t id)
    : loop_(loop), id_(id), socket_(loop->ios()) {}
Session::~Session() {}
void Session::start() {
  socket_.non_blocking(true);
  read();
}

void Session::read() {
  auto self(shared_from_this());
  socket_.async_read_some(
      asio::buffer(readbuf_.beginWrite(), readbuf_.writableBytes()),
      [self, this](const asio::error_code &err, size_t size) {
        if (!err) {
          std::cout << "recieve data" << std::endl;
          readbuf_.hasWritten(size);
          messagecallback_(self, &readbuf_);
          // std::cout<<readbuf_.peek();
          read();
        } else {
          std::cout << "read errer :" << err.message() << std::endl;
          return;
        }
      });
}

void Session::send() {
  socket_.async_write_some(
      asio::buffer(writebuf_.peek(), writebuf_.readableBytes()),
      [this](const asio::error_code &err, size_t size) {
        if (!err) {
        } else {
          std::cout << "write errer :" << err.message() << std::endl;
          return;
        }
      });
}