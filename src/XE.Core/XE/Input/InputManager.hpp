
#ifndef __XE_INPUT_INPUTMANAGER_HPP__
#define __XE_INPUT_INPUTMANAGER_HPP__

#include <map>
#include <XE/Math/Vector.hpp>

namespace XE::Input {
    enum class InputState;
    enum class InputCode;

    /**
     * @brief Handles all the different input events 
     */
    class InputManager {
    public:
        virtual ~InputManager();

        virtual InputState GetState(const InputCode code) const = 0;

        virtual XE::Math::Vector2i GetMousePosition() = 0;

        virtual void Poll() = 0;
    };
}

#endif
