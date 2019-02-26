/**
 * XE Tech demo
 */

#include <XE.hpp>
#include <XE/IO.hpp>
#include <XE/Input.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL.hpp>

#include <map>

namespace XE {
    class ResourceManager {
    public:
        std::string getString(const std::string &path) const {
            
        }
        
    private:
        std::vector<std::unique_ptr<XE::StreamSource>> streamSources;
    };
}

int main(int argc, char **argv) {
    auto graphicsDevice = std::make_unique<XE::GraphicsDeviceGL>();
    auto window = graphicsDevice->GetWindow();

    auto inputManager = graphicsDevice->GetInputManager();
    
    bool done = true;

    while (done) {
        inputManager->Poll();
        auto keyboardStatus = inputManager->GetKeyboardStatus();
        if (keyboardStatus.GetState(XE::KeyCode::KeyEsc) == XE::BinaryState::Press) {
            done = false;
        }

        graphicsDevice->beginFrame(XE::ClearFlags::All, {0.0f, 0.0f, 1.0f, 0.0f}, 0.0f, 0);

        graphicsDevice->endFrame();
    }

    return 0;
}
