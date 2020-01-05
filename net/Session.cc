#include "Session.h"
#include <iostream>
#include "Loop.h"

Session::Session(LoopPtr loop, uint64_t id) : id_(id), loop_(loop), socket_(loop->ios()) {}
Session::~Session() {}

void Session::start() {
    socket_.non_blocking(true);
    asio::ip::tcp::no_delay nodelay(true);
    socket_.set_option(nodelay);
    socket_.set_option(asio::socket_base::keep_alive(true));
    if(connectioncallback_){
        connectioncallback_(shared_from_this());
    }
    read();
}

void Session::read() {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(read_buf_.beginWrite(), read_buf_.writableBytes()), [self, this](const asio::error_code &err, size_t size) {
        if (!err) {
            // std::cout << "recieve data" << std::endl;
            read_buf_.hasWritten(size);
            messagecallback_(self, std::make_shared<Buffer>(read_buf_));
            read_buf_.retrieve(size);
            read();
        } else {
            std::cout << "read errer :" << err.message() << std::endl;
            return;
        }
    });
}

void Session::send(BufferPtr buffer) {
    {
        std::lock_guard<std::mutex> guard(mutex_);
        unsend_queue_.push_back(buffer);
        // TODO(shenyu): race condition?
        if (write_buf_.readableBytes() != 0) return;
    }

    auto self(shared_from_this());
    loop_->runInLoop([this, self]() {
        bool write_in_progress = (write_buf_.readableBytes() != 0);
        if (!write_in_progress) {
            write();
        }
    });
}

void Session::write() {
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tmp_queue_.swap(unsend_queue_);
        unsend_queue_.clear();
        if (tmp_queue_.empty()) return;
    }

    for (const auto &buf : tmp_queue_) {
        write_buf_.append(buf->peek(), buf->readableBytes());
    }

    auto self(shared_from_this());
    socket_.async_write_some(asio::buffer(write_buf_.peek(), write_buf_.readableBytes()), [self, this](const asio::error_code &err, size_t size) {
        if (!err) {
            write_buf_.retrieve(write_buf_.readableBytes());
            {
                std::lock_guard<std::mutex> guard(this->mutex_);
                if (this->unsend_queue_.empty()) return;
            }
            write();
        } else {
            std::cout << "write errer :" << err.message() << std::endl;
            return;
        }
    });
}

void Session::connect(const string &ip, int port) {
    //asio::ip::tcp::resolver resolver(loop_->ios());
    //auto ep = resolver.resolve(ip, std::to_string(port)).begin();
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
    auto self(shared_from_this());
    socket_.async_connect(ep, [self, this](const asio::error_code &err) {
        if (!err) {
            start();
        } else {
            return;
        }
    });
}

void Session::close() {
    auto self(shared_from_this());
    loop_->runInLoop([this, self]() { socket_.close(); });
}