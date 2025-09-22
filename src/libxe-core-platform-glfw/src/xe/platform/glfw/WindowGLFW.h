
#ifndef __XE_GRAPHICS_WINDOWGLFW_HPP__
#define XE_GRAPHICS_WINDOWGLFW_HPP_

#include <memory>
#include <string>
#include <xe/graphics/GraphicsContext.h>
#include <xe/graphics/GraphicsDevice.h>
#include <xe/graphics/IWindow.h>
#include <xe/input/InputManager.h>

namespace XE {
    /**
     * Abstraction layer around a GLFW Window.
     */
    class WindowGLFW : public IWindow {
    public:
        ~WindowGLFW() override;

        Vector2i getSizeInPixels() const override = 0;

        virtual GraphicsContext *getContext() const = 0;

        virtual InputManager *getInputManager() const = 0;

    
        static std::unique_ptr<WindowGLFW> create(const XE::GraphicsContext::Descriptor &descriptor, const std::string &title, const Vector2i &windowSize, bool fullScreen);
    };
} // namespace XE

#endif
