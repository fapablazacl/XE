
#ifndef __XE_GRAPHICS_IMAGE_HPP__
#define __XE_GRAPHICS_IMAGE_HPP__

#include <cstddef>
#include <XE/Predef.h>
#include <XE/Math/Vector.h>
#include <XE/Graphics/PixelFormat.h>

namespace XE {
    /**
     * @brief Abstract image class with basic image manipulation.
     */
    class Image {
    public:
        virtual ~Image();

        /**
         * @brief Get a direct pointer to the image data resident in system memory.
         */
        virtual const void* getPointer() const = 0;

        /**
         * @brief Get the image pixel format.
         */
        virtual PixelFormat getFormat() const = 0;

        /**
         * @brief Get the image dimensions as a 2D Integer Vector, in pixels.
         */
        virtual Vector2i getSize() const = 0;
    };
}

#endif
