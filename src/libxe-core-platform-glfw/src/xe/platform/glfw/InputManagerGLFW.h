
#ifndef __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__
#define XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP_

#include <GLFW/glfw3.h>
#include <xe/input/DeviceStatus.h>
#include <xe/input/InputManager.h>

namespace XE {
    class InputManagerGLFW : public InputManager {
    public:
        explicit InputManagerGLFW(GLFWwindow *window);

        void poll() override;

        KeyboardStatus getKeyboardStatus() override {
            return m_keyboardStatus;
        }

        MouseStatus getMouseStatus() override;

        int getControllerCount() override;

        ControllerStatus getControllerStatus(int index) override;

    private:
        void fillKeyboardStatus();

    
        GLFWwindow *m_windowGLFW = nullptr;
        KeyboardStatus m_keyboardStatus = {};
    };
} // namespace XE

#endif
