//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_MESSAGEBUS_H
#define XE_MESSAGEBUS_H

#include <vector>

struct Message {
    enum {
        GAME_EXIT
    } type;
};

enum class MessageBusPostMessageOptions {
    Async,
    Sync
};

class System;
class MessageBus {
public:
    MessageBus() {}

    ~MessageBus() {}

    void registerSystem(System &system);

    void postMessage(const Message &message, const MessageBusPostMessageOptions options = MessageBusPostMessageOptions::Async);

    void processPendingMessages();

private:
    void processMessage(const Message &message);

private:
    std::vector<Message> messages;
    std::vector<System*> systems;
};


#endif // XE_MESSAGEBUS_H
