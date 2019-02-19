
#ifndef __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__
#define __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__

#include <XE/Input/InputManager.hpp>
#include <XE/Input/DeviceStatus.hpp>
#include <GLFW/glfw3.h>

namespace XE {
    class InputManagerGLFW : public InputManager {
    public:
        explicit InputManagerGLFW(GLFWwindow *window);

        virtual void Poll() override;

        virtual KeyboardStatus GetKeyboardStatus() override {
            return m_keyboardStatus;
        }

        virtual MouseStatus GetMouseStatus() override;

        virtual int GetControllerCount() override;
        
        virtual ControllerStatus GetControllerStatus(const int index) override;

    private:
        void FillKeyboardStatus();

    private:
        GLFWwindow *m_windowGLFW;
        KeyboardStatus m_keyboardStatus;
    };
}

#endif
