
#pragma once

namespace XE {
    class GraphicsDevice;
    class GraphicsDeviceFactory {
    public:
        virtual ~GraphicsDeviceFactory() {}

        virtual GraphicsDevice *createDevice();
    };
} // namespace XE
