
#include <iostream>

#include <XE/UI/Frame.hpp>
#include <XE/Graphics/GraphicsDevice.hpp>

namespace XE::UI {
    class WindowSystem {
    public:
        static WindowSystem *New() {
            return new WindowSystem();
        }

        virtual Frame* CreateFrame() {
            class FrameImpl : public Frame {
            public:

            };

            return new FrameImpl();
        }
    };
}

int main(int argc, char **argv) {
    XE::UI::WindowSystem *windowSystem = XE::UI::WindowSystem::New();
    XE::UI::Frame *frame = windowSystem->CreateFrame();

    XE::Graphics::GraphicsDevice *graphicsDevice = nullptr;

    return 0;
}
