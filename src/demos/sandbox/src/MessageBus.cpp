//
// Created by Felipe Apablaza on 27-03-23.
//

#include "MessageBus.h"

#include "ECS.h"

void MessageBus::registerSystem(System &system) {
    systems.push_back(&system);
}
void MessageBus::postMessage(const Message &message, const MessageBusPostMessageOptions options) {
    switch (options) {
    case MessageBusPostMessageOptions::Async:
        messages.push_back(message);
        break;

    case MessageBusPostMessageOptions::Sync:
        processMessage(message);
        break;
    }
}

void MessageBus::processPendingMessages() {
    for (const Message &message : messages) {
        processMessage(message);
    }

    messages.clear();
}
void MessageBus::processMessage(const Message &message) {
    for (System *system : systems) {
        system->handleMessage(message);
    }
}
