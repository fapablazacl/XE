
#include <XE/Graphics/GL/GLFW/GraphicsContextGLFW.hpp>

#include <cassert>

namespace XE {
    GraphicsContextGLFW::GraphicsContextGLFW(GLFWwindow *glfwWindow, Descriptor descriptor) {
        assert(glfwWindow);

        this->glfwWindow = glfwWindow;
        this->descriptor = descriptor;
    }


    GraphicsContextGLFW::~GraphicsContextGLFW() {}


    IGraphicsContextGL::GetProcAddress GraphicsContextGLFW::getProcAddressFunction() const {
        return (GetProcAddress)glfwGetProcAddress;
    }


    IGraphicsContextGL::Descriptor GraphicsContextGLFW::getDescriptor() const {
        return descriptor;
    }


    void GraphicsContextGLFW::present() {
        ::glfwSwapBuffers(glfwWindow);
    }
}
