/**
 * XE Tech demo
 */

#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Input.hpp>
#include <XE/Graphics/GL.hpp>

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

        graphicsDevice->BeginFrame(XE::ClearFlags::All, {0.0f, 0.0f, 1.0f, 0.0f}, 0.0f, 0);

        graphicsDevice->EndFrame();
    }

    return 0;
}
