#include "chat_session.hpp"
#include <iostream>
#include <cstring>

ChatSession::ChatSession(tcp::socket socket)
    : ws_(std::move(socket)), closed_(false) {
		std::cout << "BangVV 1.1.1.1" << std::endl;
	}

void ChatSession::start(std::function<void(std::shared_ptr<ChatSession>)> on_ready,
                        std::function<void(std::shared_ptr<ChatSession>)> on_close) {
    on_close_ = on_close;

    ws_.async_accept([self = shared_from_this(), on_ready](boost::beast::error_code ec) {
        if (!ec) {
            on_ready(self);
            self->read();
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
            self->close();
        }
    });
}

void ChatSession::set_peer(std::shared_ptr<ChatSession> peer) {
    std::lock_guard<std::mutex> lock(peer_mutex_);
    peer_ = peer;
}

void ChatSession::remove_peer() {
    std::lock_guard<std::mutex> lock(peer_mutex_);
    peer_.reset();
}

void ChatSession::read() {
    auto self = shared_from_this();
    ws_.async_read(buffer_, [self](boost::beast::error_code ec, std::size_t) {
        if (!ec) {
            self->send_to_peer();
            self->read();
        } else {
            std::cerr << "Read error or connection closed: " << ec.message() << "\n";
            self->close();
        }
    });
}

void ChatSession::send_to_peer() {
    std::shared_ptr<ChatSession> peer_copy;
    {
        std::lock_guard<std::mutex> lock(peer_mutex_);
        peer_copy = peer_;
    }
    if (peer_copy && peer_copy->is_open()) {
        auto data = buffer_.data();
        auto buf_copy = std::make_shared<boost::beast::flat_buffer>();
        auto size = data.size();

        auto dest = buf_copy->prepare(size);
        std::memcpy(dest.data(), data.data(), size);
        buf_copy->commit(size);

        peer_copy->ws_.text(ws_.got_text());
        peer_copy->ws_.async_write(
            buf_copy->data(),
            [buf_copy](boost::beast::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "Write to peer error: " << ec.message() << "\n";
                }
            });
    }
    buffer_.consume(buffer_.size());
}

bool ChatSession::is_open() const {
    return ws_.is_open() && !closed_;
}

void ChatSession::close() {
    bool expected = false;
    if (closed_.compare_exchange_strong(expected, true)) {
        boost::beast::error_code ec;
        ws_.close(websocket::close_code::normal, ec);
        if (ec) {
            std::cerr << "Error on close: " << ec.message() << "\n";
        }
        if (on_close_) {
            on_close_(shared_from_this());
        }
        remove_peer();
    }
}
