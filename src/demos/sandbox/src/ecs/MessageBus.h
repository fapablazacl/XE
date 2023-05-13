//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_MESSAGEBUS_H
#define XE_MESSAGEBUS_H

#include <vector>

struct Message {
    //! The specific message type
    enum { GAME_EXIT, SYSTEM_UPDATE } type;

    //! Delta time for the current frame
    float dt = 0.0f;
};

enum class MessageBusPostMessageOptions { Async, Sync };

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
    std::vector<System *> systems;
};

#endif // XE_MESSAGEBUS_H
