
#include "DeviceStatus.hpp"

namespace XE {
    BinaryState KeyboardStatus::getState(KeyCode code) const {
        return m_keys[static_cast<int>(code)];
    }

    void KeyboardStatus::SetState(const KeyCode code, const BinaryState state) {
        m_keys[static_cast<int>(code)] = state;
    }


    Vector2i MouseStatus::GetPosition() const {
        return m_position;
    }

    void MouseStatus::SetPosition(const Vector2i &position) {
        m_position = position;
    }

    BinaryState MouseStatus::getState(const MouseButton button) const {
        return m_buttons[static_cast<int>(button)];
    }

    void MouseStatus::SetState(const MouseButton button, const BinaryState state) {
        m_buttons[static_cast<int>(button)] = state;
    }


    BinaryState ControllerStatus::getState(const ControllerButton button) const {
        return m_buttons[static_cast<int>(button)];
    }

    void ControllerStatus::SetState(const ControllerButton button, const BinaryState state) {
        m_buttons[static_cast<int>(button)] = state;
    }

    Vector2f ControllerStatus::getState(const ControllerJoystick joystick) const {
        return m_joysticks[static_cast<int>(joystick)];
    }

    void ControllerStatus::SetState(const ControllerJoystick joystick, const Vector2f &state) {
        m_joysticks[static_cast<int>(joystick)] = state;
    }
}
