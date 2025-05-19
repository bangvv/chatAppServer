#include "chat_server.hpp"
#include <iostream>

ChatServer::ChatServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint)
    : acceptor_(ioc, endpoint) {
		std::cout << "BangVV 1.1" << std::endl;
	std::cout << "BangVV 1.2" << std::endl;
}

void ChatServer::accept() {
    auto self = shared_from_this();
	std::cout << "BangVV 1.1.1" << std::endl;
    acceptor_.async_accept([self](boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            auto session = std::make_shared<ChatSession>(std::move(socket));
            session->start(
                [self](std::shared_ptr<ChatSession> session) {
                    self->handle_new_connection(session);
                },
                [self](std::shared_ptr<ChatSession> session) {
                    self->handle_session_close(session);
                }
            );
        } else {
            std::cerr << "Accept error: " << ec.message() << "\n";
        }
        self->accept(); // tiếp tục chấp nhận kết nối mới
    });
	std::cout << "BangVV 1.1.3" << std::endl;
}

void ChatServer::handle_new_connection(std::shared_ptr<ChatSession> session) {
	std::cout << "BangVV 1.1.2" << std::endl;
    std::lock_guard<std::mutex> lock(waiting_mutex_);
    if (!waitingQueue_.empty()) {
        auto peer = waitingQueue_.front();
        waitingQueue_.pop();

        auto room = std::make_shared<ChatRoom>(peer, session);
        {
            std::lock_guard<std::mutex> lock2(active_rooms_mutex_);
            activeRooms_.push_back(room);
        }
        std::cout << "Matched 2 users into a room!\n";
    } else {
        waitingQueue_.push(session);
        std::cout << "User added to waiting queue\n";
    }
}

void ChatServer::handle_session_close(std::shared_ptr<ChatSession> session) {
    std::cout << "Session closed, cleaning up...\n";

    {
        std::lock_guard<std::mutex> lock(waiting_mutex_);
        std::queue<std::shared_ptr<ChatSession>> tempQueue;
        while (!waitingQueue_.empty()) {
            auto s = waitingQueue_.front();
            waitingQueue_.pop();
            if (s != session) {
                tempQueue.push(s);
            }
        }
        std::swap(waitingQueue_, tempQueue);
    }

    {
        std::lock_guard<std::mutex> lock(active_rooms_mutex_);
        auto it = activeRooms_.begin();
        while (it != activeRooms_.end()) {
            (*it)->remove_session(session);
            if (!(*it)->is_active()) {
                it = activeRooms_.erase(it);
                std::cout << "Removed inactive room\n";
            } else {
                ++it;
            }
        }
    }
}
