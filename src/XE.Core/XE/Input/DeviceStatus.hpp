
#ifndef __XE_INPUT_INPUTSTATE_HPP__
#define __XE_INPUT_INPUTSTATE_HPP__

#include <XE/Math/Vector.hpp>

namespace XE {
    enum class BinaryState {
        Press, Release
    };

    enum class KeyCode {
        KeyEsc, KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12,
        Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, Key0, KeyBackspace,
        KeyTab, KeyQ, KeyW, KeyE, KeyR, KeyT, KeyY, KeyU, KeyI, KeyO, KeyP,
        KeyA, KeyS, KeyD, KeyF, KeyG, KeyH, KeyJ, KeyK, KeyL,
        KeyZ, KeyX, KeyC, KeyV, KeyB, KeyN, KeyM,
        KeyEnter,
        KeyLCtrl, KeyAlt, KeySpace, KeyAltGr, KeyRCtrl, KeyLShift, KeyRShift,
        KeyUp, KeyDown, KeyLeft, KeyRight,
        KeyHome, KeyEnd, KeySupr, KeyInsert, Max
    };

    class KeyboardStatus {
    public:
        BinaryState GetState(KeyCode code) const;

        void SetState(const KeyCode code, const BinaryState state);

    private:
        BinaryState m_keys[static_cast<int>(KeyCode::Max)];
    };

    enum class MouseButton {
        Left, Right, Middle, Max
    };

    class MouseStatus {
    public:
        XE::Vector2i GetPosition() const;

        void SetPosition(const XE::Vector2i &position);

        BinaryState GetState(const MouseButton button) const;

        void SetState(const MouseButton button, const BinaryState state);

    private:
        XE::Vector2i m_position;
        BinaryState m_buttons[static_cast<int>(MouseButton::Max)];
    };

    enum class ControllerButton {
        LPad_Left, LPad_Right, LPad_Up, LPad_Down,
        RPad_Left, RPad_Right, RPad_Up, RPad_Down,
        L1, R1, 
        L2, R2,
        Start, Select,
        Max
    };

    enum class ControllerJoystick {
        Left, Right, Max
    };

    class ControllerStatus {
    public:
        BinaryState GetState(const ControllerButton button) const;

        void SetState(const ControllerButton button, const BinaryState state);

        XE::Vector2f GetState(const ControllerJoystick joystick) const;

        void SetState(const ControllerJoystick joystick, const XE::Vector2f &state);

    private:
        BinaryState m_buttons[static_cast<int>(ControllerButton::Max)];
        XE::Vector2f m_joysticks[static_cast<int>(ControllerJoystick::Max)];
    };
}

#endif
