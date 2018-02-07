
#ifndef __XE_INPUT_INPUTMANAGER_HPP__
#define __XE_INPUT_INPUTMANAGER_HPP__

#include <map>
#include <XE/Math/Vector.hpp>

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

        virtual KeyboardStatus GetKeyboardStatus() const = 0;

        virtual MouseStatus GetMouseStatus() const = 0;

        virtual int GetControllerCount() const = 0;
        
        virtual ControllerStatus GetControllerStatus(const int index) const = 0;
    };
}

#endif
