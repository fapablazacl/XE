
#ifndef __XE_GRAPHICS_IGRAPHICSCONTEXT_ES2_HPP__
#define __XE_GRAPHICS_IGRAPHICSCONTEXT_ES2_HPP__

namespace XE {
    /**
     * @brief Interface that abstracts the underlying graphics context
     */
    class IGraphicsContextES2 {
    public:
        typedef void (*ProcAddress) ();
        typedef ProcAddress (*GetProcAddress)(const char *);

        /**
         * @brief Interface that abstracts the underlying graphics context
         */
        struct [[deprecated]]  Descriptor {
            int major = 0;
            int minor = 0;

            bool doubleBuffer = false;
            bool coreProfile = false;

            int redBits = 0;
            int greenBits = 0;
            int blueBits = 0;
            int alphaBits = 0;

            int depthBits = 0;
            int stencilBits = 0;

            [[deprecated]] 
            static Descriptor defaultES2();
        };

        virtual ~IGraphicsContextES2();

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
