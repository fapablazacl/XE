
#ifndef __XE_GRAPHICS_IGRAPHICSCONTEXTGL_HPP__
#define __XE_GRAPHICS_IGRAPHICSCONTEXTGL_HPP__

namespace XE {
    /**
     * @brief Interface that abstracts the underlying graphics context
     */
    class IGraphicsContextGL {
    public:
        typedef void (*ProcAddress) ();
        typedef ProcAddress (*GetProcAddress)(const char *);

        /**
         * @brief Interface that abstracts the underlying graphics context
         */
        struct Descriptor {
            int major;
            int minor;

            bool doubleBuffer;
            bool coreProfile;

            int redBits;
            int greenBits;
            int blueBits;
            int alphaBits;

            int depthBits;
            int stencilBits;
        };

        virtual ~IGraphicsContextGL();

        /**
         * @brief Gets the specific proc address function specific to the context.
         */
        virtual GetProcAddress getProcAddressFunction() const = 0;

        /**
         * @brief Gets a structure that describes the context in detail.
         */
        virtual Descriptor getDescriptor() const = 0;

        /**
         * @brief Swaps the buffers and present the results to the Screen
         */
        virtual void present() = 0;
    };
}

#endif
