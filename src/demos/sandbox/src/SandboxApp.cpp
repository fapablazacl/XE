

#include "SandboxApp.h"
#include "Assets.h"


namespace Sandbox {
    SandboxApp::SandboxApp(int, char **) {
        mAssetManager = std::make_shared<XE::AssetManager>(&mAssetManagerLogger);

        mAssetManager->addAssetPack("core", XE::AssetPack {
            std::string(XE_SANDBOX_ROOT_PATH), {
                {ASSET_MODEL_SIMPLE_CUBE, "assets/models/textured-cube.glb"},
                {ASSET_SHADER_MAIN_VERT, "assets/shaders/gl4/gl4-main.vert"},
                {ASSET_SHADER_MAIN_FRAG, "assets/shaders/gl4/gl4-main.frag"},
            }
        });

        renderingSystem = std::make_shared<RenderingSystem>(&renderingSystemLogger, mAssetManager.get());
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
