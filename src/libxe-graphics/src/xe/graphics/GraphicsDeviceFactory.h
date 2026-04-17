
#pragma once

namespace xe {
    class GraphicsDevice;
    class GraphicsDeviceFactory {
    public:
        virtual ~GraphicsDeviceFactory() {
        }

        virtual GraphicsDevice *createDevice();
    };
} // namespace xe
