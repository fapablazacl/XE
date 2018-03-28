
#ifndef __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__
#define __XE_GRAPHICS_GL_INPUTMANAGERGLFW_HPP__

#include <XE/Input/InputManager.hpp>
#include <XE/Input/DeviceStatus.hpp>
#include <GLFW/glfw3.h>

namespace XE::Graphics::GL {
    class InputManagerGLFW : public XE::Input::InputManager {
    public:
        explicit InputManagerGLFW(GLFWwindow *window);

        virtual void Poll() override;

        virtual XE::Input::KeyboardStatus GetKeyboardStatus() override {
            return m_keyboardStatus;
        }

        virtual XE::Input::MouseStatus GetMouseStatus() override;

        virtual int GetControllerCount() override;
        
        virtual XE::Input::ControllerStatus GetControllerStatus(const int index) override;

    private:
        void FillKeyboardStatus();

    private:
        GLFWwindow *m_window;
        XE::Input::KeyboardStatus m_keyboardStatus;
    };
}

#endif
