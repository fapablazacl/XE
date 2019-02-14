
#ifndef __XE_GRAPHICS_IMAGE_HPP__
#define __XE_GRAPHICS_IMAGE_HPP__

#include <cstddef>
#include <XE/Predef.hpp>
#include <XE/Math/Vector.hpp>
#include <XE/Graphics/PixelFormat.hpp>

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
        virtual const std::byte* GetPointer() const = 0;

        /**
         * @brief Get the image pixel format.
         */
        virtual PixelFormat GetFormat() const = 0;

        /**
         * @brief Get the image dimensions as a 2D Integer Vector, in pixels.
         */
        virtual Vector2i GetSize() const = 0;
    };
}

#endif
