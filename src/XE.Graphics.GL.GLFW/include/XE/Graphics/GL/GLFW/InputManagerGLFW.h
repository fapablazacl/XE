
#ifndef __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__
#define __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__

#include <XE/Input/InputManager.h>
#include <XE/Input/DeviceStatus.h>
#include <GLFW/glfw3.h>

namespace XE {
    class InputManagerGLFW : public InputManager {
    public:
        explicit InputManagerGLFW(GLFWwindow *window);

        virtual void poll() override;

        virtual KeyboardStatus getKeyboardStatus() override {
            return m_keyboardStatus;
        }

        virtual MouseStatus getMouseStatus() override;

        virtual int getControllerCount() override;
        
        virtual ControllerStatus getControllerStatus(const int index) override;

    private:
        void fillKeyboardStatus();

    private:
        GLFWwindow *m_windowGLFW = nullptr;
        KeyboardStatus m_keyboardStatus;
    };
}

#endif
