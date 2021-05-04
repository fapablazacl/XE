
#ifndef __XE_GRAPHICS_WINDOWGLFW_HPP__
#define __XE_GRAPHICS_WINDOWGLFW_HPP__

#include <string>
#include <memory>
#include <XE/Input/InputManager.hpp>
#include <XE/Graphics/IWindow.hpp>
#include <XE/Graphics/GL/IGraphicsContextGL.hpp>

namespace XE {
    typedef IGraphicsContextGL::Descriptor ContextDescriptorGL;

    /**
     * Abstraction layer around a GLFW Window.
     */
    class WindowGLFW : public IWindow {
    public:
        virtual ~WindowGLFW();

        virtual Vector2i getSize() const = 0;

        virtual IGraphicsContextGL* getContext() const = 0;

        virtual InputManager* getInputManager() const = 0;

    public:
        static std::unique_ptr<WindowGLFW> create(const ContextDescriptorGL &contextDescriptor, const std::string &title, const Vector2i &windowSize, const bool fullScreen);
    };
}

#endif
