#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "chat_session.hpp"
#include "chat_room.hpp"
#include <boost/asio.hpp>
#include <queue>
#include <vector>
#include <memory>
#include <mutex>

class ChatServer : public std::enable_shared_from_this<ChatServer> {
public:
    ChatServer(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint);
	void start() {
        accept();
    }
private:
    void accept();
    void handle_new_connection(std::shared_ptr<ChatSession> session);
    void handle_session_close(std::shared_ptr<ChatSession> session);

    boost::asio::ip::tcp::acceptor acceptor_;
    std::queue<std::shared_ptr<ChatSession>> waitingQueue_;
    std::vector<std::shared_ptr<ChatRoom>> activeRooms_;
    std::mutex waiting_mutex_;
    std::mutex active_rooms_mutex_;
};

#endif // CHAT_SERVER_HPP
