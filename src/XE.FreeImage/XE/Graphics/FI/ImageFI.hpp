
#ifndef __XE_GRAPHICS_IMAGEFI_HPP__
#define __XE_GRAPHICS_IMAGEFI_HPP__

#include <XE/Graphics/Image.hpp>

namespace XE::Graphics::FI {
    /**
     * @brief A Abstract Image class that's used for an abstraction around image classes from another libraries.
     */
    class ImageFI : public Image {
    public:
        virtual ~ImageFI();

        virtual std::byte* GetPointer() const = 0;

        virtual PixelFormat GetFormat() const = 0;

        virtual XE::Math::Vector2i GetSize() const = 0;
    };
}

#endif
