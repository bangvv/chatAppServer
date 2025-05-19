#pragma once
#include <memory>

class ChatSession;

class ChatRoom {
public:
    ChatRoom(std::shared_ptr<ChatSession> a, std::shared_ptr<ChatSession> b);

    bool contains(std::shared_ptr<ChatSession> session);
    void remove_session(std::shared_ptr<ChatSession> session);
    bool is_active() const;

private:
    std::weak_ptr<ChatSession> session_a_;
    std::weak_ptr<ChatSession> session_b_;
};
