

#include "SandboxApp.h"

namespace Sandbox {
    SandboxApp::SandboxApp(int, char **) {
        renderingSystem = std::make_shared<RenderingSystem>(&renderingSystemLogger);
    }

    void SandboxApp::initialize() {
        renderingSystem->Initialize();
    }

    void SandboxApp::update(const float seconds) {
        renderingSystem->Update(seconds);
    }

    void SandboxApp::render() {
        renderingSystem->Render();
    }

    bool SandboxApp::shouldClose() const {
        return renderingSystem->ShouldClose();
    }
} // namespace Sandbox
