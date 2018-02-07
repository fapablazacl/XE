
#ifndef __XE_UI_FRAME_HPP__
#define __XE_UI_FRAME_HPP__

#include <cstddef>

namespace XE::Input {
    class InputManager;
}

namespace XE::UI {
    /**
     * @brief Abstract a (possible) native OS window frame, for presenting rendering content to the user.
     */
    class Frame {
    public:
        virtual XE::Input::InputManager* GetInputManager() = 0;
    };
}

#endif
