
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include <memory>

#include "ECS.h"
#include "PhysicsSystem.h"

namespace Sandbox {
    class SandboxApp {
    public:
        explicit SandboxApp(int argc, char **argv);

        void initialize();

        void Update(const float seconds);

        void Render();

        bool ShouldClose() const;

    private:
        bool running = true;
        int frames = 0;
        const int maxFrames = 10000;
        Coordinator coordinator;
        std::shared_ptr<PhysicsSystem> physicsSystem;
    };
} // namespace Sandbox

#endif
