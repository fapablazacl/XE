
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
                virtual Widget* GetChild(const int index) override {
                    return nullptr;
                }

                virtual const Widget* GetChild(const int index) const  override {
                    return nullptr;
                }

                virtual int GetChildCount() const  override {
                    return 0;
                }

                virtual WidgetProperties GetProperties() const  override {
                    return WidgetProperties();
                }

                virtual void SetProperties(const WidgetProperties &properties)  override {
                    
                }

                virtual XE::Input::InputManager* GetInputManager() override {
                    return nullptr;
                }
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
