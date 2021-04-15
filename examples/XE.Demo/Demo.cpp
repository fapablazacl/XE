/**
 * XE Tech demo
 */

#include <XE/XE.hpp>
#include <XE/IO.hpp>
#include <XE/Input.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL.hpp>
#include <XE/Graphics/GL/GLFW/WindowGLFW.hpp>

#include <map>

namespace XE {
    class ResourceManager {
    public:
        std::string getString(const std::string &path) const {}
        
    private:
        std::vector<std::unique_ptr<XE::StreamSource>> streamSources;
    };
}

int main(int argc, char **argv) {
    auto window = XE::WindowGLFW::create(XE::ContextDescriptorGL::defaultGL4(), "XE.Demo example application", {1200, 800}, false);
    auto graphicsDevice = std::make_unique<XE::GraphicsDeviceGL>(window->getContext());
    auto inputManager = window->getInputManager();
    
    bool done = true;

    while (done) {
        inputManager->poll();
        auto keyboardStatus = inputManager->getKeyboardStatus();
        if (keyboardStatus.getState(XE::KeyCode::KeyEsc) == XE::BinaryState::Press) {
            done = false;
        }

        graphicsDevice->beginFrame(XE::ClearFlags::All, {0.0f, 0.0f, 1.0f, 0.0f}, 0.0f, 0);

        graphicsDevice->endFrame();
    }

    return 0;
}
