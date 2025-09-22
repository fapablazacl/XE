
#ifndef __XE_INPUT_INPUTMANAGER_HPP__
#define XE_INPUT_INPUTMANAGER_HPP_

namespace XE {
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

        /**
         * @brief Scans the status of all recognized input devices.
         */
        virtual void poll() = 0;

        virtual KeyboardStatus getKeyboardStatus() = 0;

        virtual MouseStatus getMouseStatus() = 0;

        virtual int getControllerCount() = 0;

        virtual ControllerStatus getControllerStatus(int index) = 0;
    };
} // namespace XE

#endif
