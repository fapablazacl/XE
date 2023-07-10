
#pragma once

#ifndef XE_INPUTSYSTEM_H
#define XE_INPUTSYSTEM_H

namespace XE {
    class InputManager;
}

class InputSystem {
public:
    explicit InputSystem();

    ~InputSystem();

public:
    void setup(XE::InputManager *inputManager);

private:
    XE::InputManager *mInputManager = nullptr;
};

#endif // XE_INPUTSYSTEM_H
