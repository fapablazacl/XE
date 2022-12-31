
#ifndef __XE_GRAPHICS_GRAPHICSCONTEXTGLFW_HPP__
#define __XE_GRAPHICS_GRAPHICSCONTEXTGLFW_HPP__

#include <xe/graphics/GraphicsContext.h>
#include <GLFW/glfw3.h>

namespace XE {
    class GraphicsContextGLFW : public GraphicsContext {
    public:
        GraphicsContextGLFW(GLFWwindow *glfwWindow, Descriptor descriptor);

        ~GraphicsContextGLFW();

        GetProcAddressGL getProcAddressFunctionGL() const override;

        Descriptor getDescriptor() const override;

        Descriptor getRequestedDescriptor() const override {
            // TODO: Add proper implementation later
            return descriptor;
        }

        void present() override;

    private:
        GLFWwindow *glfwWindow = nullptr;
        Descriptor descriptor;
    };
}

#endif
