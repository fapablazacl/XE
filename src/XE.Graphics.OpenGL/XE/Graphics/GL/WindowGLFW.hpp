
#ifndef __XE_GRAPHICS_GL_WINDOWGLFW_HPP__
#define __XE_GRAPHICS_GL_WINDOWGLFW_HPP__

#include <GLFW/glfw3.h>
#include <XE/Graphics/GraphicsDevice.hpp>

namespace XE {
    class WindowGLFW : public Window {
    public:
        explicit WindowGLFW(GLFWwindow *window);

        virtual XE::Vector2i GetSize() const override;

    private:
        GLFWwindow *m_windowGLFW;
    };
}

#endif
