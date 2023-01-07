
#pragma once

namespace XE {
    class GraphicsDevice;
    class GraphicsDeviceFactory;

    /**
     * @brief Serves
     *
     */
    class GraphicsManager {
    public:
        /**
         * @brief
         *
         * @param factory
         */
        void registerFactory(GraphicsDeviceFactory *factory);

        /**
         * @brief
         *
         * @param factory
         */
        void unregisterFactory(GraphicsDeviceFactory *factory);

        /**
         * @brief Create a default graphics device.
         *
         * This device depends on the current platform:
         *
         * For desktop platforms: OpenGL 3
         * For mobile platforms: OpenGL ES 2
         *
         * @return GraphicsDevice*
         */
        GraphicsDevice *createDevice();
    };
} // namespace XE
