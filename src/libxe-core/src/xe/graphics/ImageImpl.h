
#ifndef __XE_GRAPHICS_IMAGEIMPL_HPP__
#define XE_GRAPHICS_IMAGEIMPL_HPP_

#include "Image.h"

namespace XE {
    /**
     * @brief Image implementation class.
     */
    class ImageImpl : public Image {
    public:
        ImageImpl();

        ~ImageImpl() override;

        const void *getPointer() const override;

        PixelFormat getFormat() const override;

        Vector2i getSize() const override;

        void allocate(PixelFormat newFormat, const Vector2i &newSize);

        void free();

    private:
        void *pixels = nullptr;
        PixelFormat format = PixelFormat::Unknown;
        Vector2i size = {0, 0};
    };
} // namespace XE

#endif
