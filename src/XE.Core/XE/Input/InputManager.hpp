
#ifndef __XE_INPUT_INPUTMANAGER_HPP__
#define __XE_INPUT_INPUTMANAGER_HPP__

namespace XE::Input {
    class KeyboardStatus;
    class MouseStatus;
    class ControllerStatus;

    /**
     * @brief Provides low-level device status scanning capabilities
     *
     * @todo Add API for connected device detection
     */
    class InputManager {
    public:
        virtual ~InputManager();

        virtual KeyboardStatus GetKeyboardStatus() = 0;

        virtual MouseStatus GetMouseStatus() = 0;

        virtual int GetControllerCount() = 0;
        
        virtual ControllerStatus GetControllerStatus(const int index) = 0;
    };
}

#endif
