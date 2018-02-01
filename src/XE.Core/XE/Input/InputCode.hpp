
#ifndef __XE_INPUT_INPUTCODE_HPP__
#define __XE_INPUT_INPUTCODE_HPP__

namespace XE::Input {
    enum class InputCode {
        /* Keyboard */
        KeyEsc, KeyF1, KeyF2, KeyF3, KeyF4, KeyF5, KeyF6, KeyF7, KeyF8, KeyF9, KeyF10, KeyF11, KeyF12,
        Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, Key0, KeyBackspace,
        KeyTab, KeyQ, KeyW, KeyE, KeyR, KeyT, KeyY, KeyU, KeyI, KeyO, KeyP,
        KeyA, KeyS, KeyD, KeyF, KeyG, KeyH, KeyJ, KeyK, KeyL,
        KeyEnter,
        KeyZ, KeyX, KeyC, KeyV, KeyB, KeyN, KeyM,
        KeyLCtrl, KeyAlt, KeySpace, KeyAltGr, KeyRCtrl, KeyLShift, KeyRShift,
        KeyUp, KeyDown, KeyLeft, KeyRight,
        KeyHome, KeyEnd, KeySupr, KeyInsert,

        /* Mouse */
        ButtonLeft, ButtonRight, ButtonMiddle
    };
}

#endif
