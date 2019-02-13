
#ifndef __XE_GRAPHICS_IMAGEIMPL_HPP__
#define __XE_GRAPHICS_IMAGEIMPL_HPP__

#include "Image.hpp"

namespace XE {
    /**
     * @brief Image implementation class.
     */
    class ImageImpl : public Image {
    public:
        ImageImpl();

        virtual ~ImageImpl();

        virtual const std::byte* GetPointer() const override;
        
        virtual PixelFormat GetFormat() const override;

        virtual XE::Vector2i GetSize() const override;

        void Allocate(const PixelFormat format, const XE::Vector2i &size);

        void Free();

    private:
        std::byte *pixels = nullptr;
        PixelFormat format = PixelFormat::Unknown;
        XE::Vector2i size = {0, 0};
    };
}

#endif
