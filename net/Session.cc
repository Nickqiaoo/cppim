#include "Session.h"
#include "Loop.h"
#include "log.h"

#include <iostream>

Session::Session(LoopPtr loop, uint64_t id) : id_(id), loop_(loop), socket_(loop->ios()), timer_(loop->ios()) {}
Session::~Session() {}

void Session::start() {
    socket_.non_blocking(true);
    asio::ip::tcp::no_delay nodelay(true);
    socket_.set_option(nodelay);
    socket_.set_option(asio::socket_base::keep_alive(true));
    LOG_INFO("session conected id:{}",id_);
    if (connectioncallback_) {
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
            LOG_ERROR("read error: {}", err.message());
            if(reconnect_){
                reconnect();
            }else{
                close();
            }
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
            LOG_ERROR("write error: {}", err.message());
            return;
        }
    });
}

void Session::connect(const string &ip, int port) {
    // asio::ip::tcp::resolver resolver(loop_->ios());
    // auto ep = resolver.resolve(ip, std::to_string(port)).begin();
    ip_ = ip;
    port_ = port;
    reconnect_ = true;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip), port);
    auto self(shared_from_this());
    socket_.async_connect(ep, [self, this](const asio::error_code &err) {
        if (!err) {
            start();
        } else {
            LOG_ERROR("connect error: {}", err.message());
            socket_.close();
            reconnect();
        }
        return;
    });
}

void Session::reconnect() {
    timer_.expires_from_now(std::chrono::seconds(5));
    auto self(shared_from_this());
    timer_.async_wait([self, this](const std::error_code &err) {
        if (!err) {
            LOG_INFO("start reconnect ip:{} port:{}", ip_, port_);
            connect(ip_, port_);
        }
    });
}

void Session::close() {
    auto self(shared_from_this());
    loop_->runInLoop([this, self]() { socket_.close(); });
}