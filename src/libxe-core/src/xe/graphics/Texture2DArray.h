
#ifndef __XE_GRAPHICS_TEXTURE2DARRAY_HPP__
#define XE_GRAPHICS_TEXTURE2DARRAY_HPP_

#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    class Texture2DArray : public Texture {

    public:
        TextureType getType() const override;

    
        ~Texture2DArray() override;

        virtual Vector2i getSize() const = 0;

        virtual int getCount() const = 0;

        virtual void setData(
            const void *surfaceData, int mipLevel, int arrayIndex, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area, int count
        ) = 0;

        virtual void
        getData(void *surfaceData, int mipLevel, int arrayIndex, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area) const = 0;
    };
} // namespace XE

#endif
