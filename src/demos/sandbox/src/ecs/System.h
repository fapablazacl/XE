//
// Created by Felipe Apablaza on 27-03-23.
//

#ifndef XE_SYSTEM_H
#define XE_SYSTEM_H

#include "CommonECS.h"
#include <set>

struct Message;
class MessageBus;
class System {
public:
    explicit System(MessageBus &messageBus) : messageBus{messageBus} {}

    virtual void handleMessage(const Message &message) = 0;

public:
    std::set<Entity> entities;

private:
    MessageBus &messageBus;
};

#endif // XE_SYSTEM_H
