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
    virtual void handleMessage(const Message &message) = 0;

    virtual ~System() {};

public:
    std::set<Entity> entities;
};

#endif // XE_SYSTEM_H
