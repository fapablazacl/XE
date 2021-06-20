
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include <memory>
#include <string>
#include <vector>

namespace XE {
    class Application {
    public:
        virtual ~Application() {}

        virtual void Initialize() = 0;
        virtual void Update(const float seconds) = 0;
        virtual void Render() = 0;

        virtual bool ShouldClose() const = 0;

    public:
        static std::unique_ptr<Application> create();
    };
}

#endif
