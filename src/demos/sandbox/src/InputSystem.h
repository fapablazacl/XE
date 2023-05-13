
#ifndef XE_INPUTSYSTEM_H
#define XE_INPUTSYSTEM_H

#include "ecs/ECS.h"

namespace XE {
    class InputManager;
}

class InputSystem : public System {
public:
    explicit InputSystem(Coordinator &coordinator);

    ~InputSystem() override;

    void handleMessage(const Message &message) override;

public:
    void setup(XE::InputManager *inputManager);

private:
    Coordinator &coordinator;

    XE::InputManager *inputManager;
};

#endif // XE_INPUTSYSTEM_H
