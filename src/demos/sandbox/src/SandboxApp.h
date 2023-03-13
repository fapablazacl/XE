
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include "ECS.h"

namespace Sandbox {
    class SandboxApp {
    public:
        explicit SandboxApp(int argc, char **argv);

        void Initialize();

        void Update(const float seconds);

        void Render();

        bool ShouldClose() const;

    private:
        bool running = false;
    };
} // namespace Sandbox

#endif
