#include "chat_server.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

int main() {
    try {
        asio::io_context ioc;

        // Đúng cách: tạo bằng shared_ptr
        auto server = std::make_shared<ChatServer>(ioc, tcp::endpoint(tcp::v4(), 8080));
		server->start();
		std::cout << "BangVV 1" << std::endl;
        const int thread_count = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        threads.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&ioc]() {
                ioc.run();
            });
        }
		std::cout << "BangVV 2" << std::endl;
        std::cout << "Server is running at ws://localhost:8080\n";

        for (auto& t : threads) {
            t.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
