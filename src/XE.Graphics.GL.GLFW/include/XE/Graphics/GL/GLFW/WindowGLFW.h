
#ifndef __XE_GRAPHICS_WINDOWGLFW_HPP__
#define __XE_GRAPHICS_WINDOWGLFW_HPP__

#include <string>
#include <memory>
#include <XE/Input/InputManager.h>
#include <XE/Graphics/IWindow.h>
#include <XE/Graphics/GraphicsDevice.h>

namespace XE {
    /**
     * Abstraction layer around a GLFW Window.
     */
    class WindowGLFW : public IWindow {
    public:
        virtual ~WindowGLFW();

        virtual Vector2i getSize() const = 0;

        virtual GraphicsContext* getContext() const = 0;

        virtual InputManager* getInputManager() const = 0;

    public:
        static std::unique_ptr<WindowGLFW> create(const XE::GraphicsContext::Descriptor &descriptor, const std::string &title, const Vector2i &windowSize, const bool fullScreen);
    };
}

#endif
