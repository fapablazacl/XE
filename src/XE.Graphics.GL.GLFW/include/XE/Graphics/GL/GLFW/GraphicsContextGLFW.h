
#ifndef __XE_GRAPHICS_GRAPHICSCONTEXTGLFW_HPP__
#define __XE_GRAPHICS_GRAPHICSCONTEXTGLFW_HPP__

#include <XE/Graphics/GL/IGraphicsContextGL.h>
#include <GLFW/glfw3.h>

namespace XE {
    class GraphicsContextGLFW : public IGraphicsContextGL {
    public:
        GraphicsContextGLFW(GLFWwindow *glfwWindow, Descriptor descriptor);

        ~GraphicsContextGLFW();

        virtual GetProcAddress getProcAddressFunction() const;

        virtual Descriptor getDescriptor() const;

        virtual void present();

    private:
        GLFWwindow *glfwWindow;
        Descriptor descriptor;
    };
}

#endif
