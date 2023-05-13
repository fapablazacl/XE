
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include <memory>

#include "PhysicsSystem.h"
#include "ecs/ECS.h"

namespace Sandbox {
    class SandboxApp {
    public:
        explicit SandboxApp(int argc, char **argv);

        void initialize();

        void update(const float seconds);

        void render();

        bool shouldClose() const;

    private:
        bool running = true;
        int frames = 0;
        const int maxFrames = 10000;
        Coordinator coordinator;
        std::shared_ptr<PhysicsSystem> physicsSystem;
    };
} // namespace Sandbox

#endif
