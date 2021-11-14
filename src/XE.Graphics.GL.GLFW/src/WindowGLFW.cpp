
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>

#include <memory>
#include <GLFW/glfw3.h>
#include <XE/Graphics/GL/GLFW/InputManagerGLFW.h>
#include <XE/Graphics/GL/GLFW/GraphicsContextGLFW.h>

#include <iostream>
#include <map>


namespace XE {    
    void static errorCallback(int error, const char *description) {
        std::cout << "errorCallback: " << error << ": " << description << std::endl;
    }

    static std::map<int, int> mapToHints(const GraphicsContext::Descriptor &descriptor) {
        std::map<int, int> hints;

        auto version = get_version(descriptor.backend);

        if (descriptor.backend == GraphicsBackend::GL_41) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
        } else {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
        }

        hints[GLFW_CONTEXT_VERSION_MAJOR] = std::get<0>(version);
        hints[GLFW_CONTEXT_VERSION_MINOR] = std::get<1>(version);
        
        hints[GLFW_OPENGL_FORWARD_COMPAT] = GL_TRUE;
        hints[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_CORE_PROFILE;

        hints[GLFW_DEPTH_BITS] = get_depth_bit_count(descriptor.depthBufferFormat);
        hints[GLFW_DOUBLEBUFFER] = GL_TRUE;

        auto colorBits = get_color_bit_count(descriptor.frameBufferFormat);

        hints[GLFW_RED_BITS] = std::get<0>(colorBits);
        hints[GLFW_GREEN_BITS] = std::get<1>(colorBits);
        hints[GLFW_BLUE_BITS] = std::get<2>(colorBits);
        hints[GLFW_ALPHA_BITS] = std::get<3>(colorBits);
        hints[GLFW_RESIZABLE] = GL_FALSE;

        return hints;
    }

    class WindowGLFWImpl : public WindowGLFW {
    public:
        WindowGLFWImpl(const GraphicsContext::Descriptor &contextDescriptor, const std::string &title, const Vector2i &windowSize, const bool fullScreen) {
            if (usageCount++ == 0) {
                glfwInit();
            }

            std::cout << "[GLFW] Initializing GLFW ..." << std::endl;

            glfwSetErrorCallback(errorCallback);

            auto hints = mapToHints(contextDescriptor);

            for (auto &pair : hints) {
                glfwWindowHint(pair.first, pair.second);
            }

            std::cout << "[GLFW] Creating Window/Context ..." << std::endl;
            glfwWindow = glfwCreateWindow (
                windowSize.X, windowSize.Y, 
                title.c_str(), 
                fullScreen ? glfwGetPrimaryMonitor() : nullptr,
                nullptr
            );

            if (!glfwWindow) {
                std::string desc;
                desc.resize(256);
                
                const int error = glfwGetError((const char **)desc.c_str());

                desc = "Error code " + std::to_string(error) + ": " + desc;
                
                throw std::runtime_error(desc);
            }

            std::cout << "[GLFW] Making Context current ..." << std::endl;
            glfwMakeContextCurrent(glfwWindow);

            graphicsContext = std::make_unique<GraphicsContextGLFW>(glfwWindow, contextDescriptor);
            inputManager = std::make_unique<InputManagerGLFW>(glfwWindow);
        }

        ~WindowGLFWImpl() {
            if (glfwWindow) {
                glfwMakeContextCurrent(nullptr);
                glfwDestroyWindow(glfwWindow);
            }

            if (--usageCount == 0) {
                glfwTerminate();
            }
        }
    
        virtual Vector2i getSize() const override {
            int width, height;

            glfwGetWindowSize(glfwWindow, &width, &height);

            return {width, height};
        }

        virtual GraphicsContext* getContext() const override {
            return graphicsContext.get();
        }

        virtual InputManager* getInputManager() const override {
            return inputManager.get();
        }

    private:
        GLFWwindow *glfwWindow = nullptr;

    private:
        std::unique_ptr<GraphicsContextGLFW> graphicsContext;
        std::unique_ptr<InputManagerGLFW> inputManager;
        static int usageCount;
    };

    int WindowGLFWImpl::usageCount = 0;

    WindowGLFW::~WindowGLFW() {}

    std::unique_ptr<WindowGLFW> WindowGLFW::create(
            const GraphicsContext::Descriptor &contextDescriptor, 
            const std::string &title, 
            const Vector2i &windowSize, 
            const bool fullScreen) {
        return std::make_unique<WindowGLFWImpl>(contextDescriptor, title, windowSize, fullScreen);
    }
}
