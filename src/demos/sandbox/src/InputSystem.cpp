
#include "InputSystem.h"

#include <xe/input/DeviceStatus.h>
#include <xe/input/InputManager.h>

InputSystem::InputSystem(Coordinator &coordinator)  : coordinator(coordinator) {

}

InputSystem::~InputSystem() {}


void InputSystem::handleMessage(const Message &message) {
    if (message.type == Message::SYSTEM_UPDATE) {
        inputManager->poll();

        const auto status = inputManager->getKeyboardStatus();

        if (status.isPressed(XE::KeyCode::KeyEsc)) {
            // TODO: Post game exit action
        }
    }
}


void InputSystem::setup(XE::InputManager *inputManager) {
    assert(inputManager);

    this->inputManager = inputManager;
}
