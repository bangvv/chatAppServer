#include "chat_room.hpp"
#include "chat_session.hpp"

ChatRoom::ChatRoom(std::shared_ptr<ChatSession> a, std::shared_ptr<ChatSession> b)
    : session_a_(a), session_b_(b) {
    a->set_peer(b);
    b->set_peer(a);
}

bool ChatRoom::contains(std::shared_ptr<ChatSession> session) {
    return session_a_.lock() == session || session_b_.lock() == session;
}

void ChatRoom::remove_session(std::shared_ptr<ChatSession> session) {
    if (session_a_.lock() == session) {
        session_a_.reset();
    }
    if (session_b_.lock() == session) {
        session_b_.reset();
    }
}

bool ChatRoom::is_active() const {
    return !session_a_.expired() || !session_b_.expired();
}
