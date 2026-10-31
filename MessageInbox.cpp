#include "MessageInbox.h"

MessageInbox::MessageInbox(std::size_t maxTextBytes)
    : head(nullptr), tail(nullptr), maxTextBytes_(maxTextBytes), 
      usedTextBytes_(0), messageCount_(0) {
}

MessageInbox::~MessageInbox() {
    clear();
}

bool MessageInbox::receive(std::size_t id, const std::string& sender, const std::string& text) {
    if (id == 0 || sender.empty() || text.empty()) {
        return false;
    }

    if (usedTextBytes_ + text.size() > maxTextBytes_) {
        return false;
    }

    Node* current = head;
    while (current != nullptr) {
        if (current->msg.id == id) {
            return false;
        }
        current = current->next;
    }

    TextMessage newMsg{id, sender, text, false};
    Node* newNode = new Node(newMsg);

    if (head == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }

    usedTextBytes_ += text.size();
    messageCount_++;

    return true;
}

bool MessageInbox::read(std::size_t id, TextMessage& result) {
    Node* current = head;
    while (current != nullptr) {
        if (current->msg.id == id) {
            current->msg.isRead = true;
            result = current->msg;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool MessageInbox::erase(std::size_t id) {
    Node* current = head;
    Node* previous = nullptr;

    while (current != nullptr) {
        if (current->msg.id == id) {
            if (current == head) {
                head = current->next;
                if (head == nullptr) {
                    tail = nullptr;
                }
            } else {
                previous->next = current->next;
                if (current == tail) {
                    tail = previous;
                }
            }

            usedTextBytes_ -= current->msg.text.size();
            messageCount_--;

            delete current;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

std::vector<TextMessage> MessageInbox::messages() const {
    std::vector<TextMessage> snapshot;
    Node* current = head;
    while (current != nullptr) {
        snapshot.push_back(current->msg);
        current = current->next;
    }
    return snapshot;
}

void MessageInbox::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* to_delete = current;
        current = current->next;
        delete to_delete;
    }
    head = nullptr;
    tail = nullptr;
    usedTextBytes_ = 0;
    messageCount_ = 0;
}

std::size_t MessageInbox::remainingTextBytes() const {
    return maxTextBytes_ - usedTextBytes_;
}

std::size_t MessageInbox::usedTextBytes() const {
    return usedTextBytes_;
}

std::size_t MessageInbox::messageCount() const {
    return messageCount_;
}