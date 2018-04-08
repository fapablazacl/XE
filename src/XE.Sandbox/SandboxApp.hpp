
#ifndef __XE_SANDBOX_SANDBOXAPP_HPP__
#define __XE_SANDBOX_SANDBOXAPP_HPP__

#include <memory>
#include <string>
#include <vector>

namespace XE::Sandbox {
    class Application {
    public:
        virtual ~Application() {}

        virtual void Initialize() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;

        virtual bool ShouldClose() const = 0;

    public:
        static std::unique_ptr<Application> Create(const std::vector<std::string> &args);
    };
}

#endif
