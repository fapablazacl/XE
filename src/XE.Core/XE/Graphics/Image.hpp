
#ifndef __XE_GRAPHICS_IMAGE_HPP__
#define __XE_GRAPHICS_IMAGE_HPP__

#include <cstddef>
#include <XE/Math/Vector.hpp>
#include <XE/Graphics/PixelFormat.hpp>

namespace XE::Graphics {
    /**
     * @brief Abstract image class with basic image manipulation.
     */
    class Image {
    public:
        virtual ~Image();

        /**
         * @brief Get a direct pointer to the image data resident in system memory.
         */
        virtual std::byte* GetPointer() const = 0;

        /**
         * @brief Get the image pixel format.
         */
        virtual PixelFormat GetFormat() const = 0;

        /**
         * @brief Get the image dimensions as a 2D Integer Vector, in pixels.
         */
        virtual XE::Math::Vector2i GetSize() const = 0;
    };
}

#endif
