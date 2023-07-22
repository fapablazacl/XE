
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include <memory>

#include "RenderingSystem.h"
#include "InputSystem.h"
#include "AssetManager.h"
#include "Logger.h"


namespace Sandbox {
    class SandboxApp {
    public:
        explicit SandboxApp(int argc, char **argv);

        void initialize();

        void update(const float seconds);

        void render();

        bool shouldClose() const;

    private:
        std::shared_ptr<AssetManager> mAssetManager;
        Logger mAssetManagerLogger{"AssetManager"};

        std::shared_ptr<RenderingSystem> renderingSystem;
        Logger renderingSystemLogger{"RenderingSystem"};

        std::shared_ptr<InputSystem> inputSystem;
    };
} // namespace Sandbox

#endif
