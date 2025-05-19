#pragma once
#include <boost/asio.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>

namespace asio = boost::asio;
namespace websocket = boost::beast::websocket;
using tcp = asio::ip::tcp;

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    explicit ChatSession(tcp::socket socket);

    void start(std::function<void(std::shared_ptr<ChatSession>)> on_ready,
               std::function<void(std::shared_ptr<ChatSession>)> on_close);

    void set_peer(std::shared_ptr<ChatSession> peer);
    void remove_peer();

    bool is_open() const;
    void close();

private:
    void read();
    void send_to_peer();

    websocket::stream<tcp::socket> ws_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<ChatSession> peer_;
    mutable std::mutex peer_mutex_;

    std::function<void(std::shared_ptr<ChatSession>)> on_close_;
    std::atomic<bool> closed_;
};
