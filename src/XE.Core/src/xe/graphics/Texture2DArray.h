
#ifndef __XE_GRAPHICS_TEXTURE2DARRAY_HPP__
#define __XE_GRAPHICS_TEXTURE2DARRAY_HPP__

#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    class Texture2DArray : public Texture {

    public:
        virtual TextureType getType() const override;

    public:
        virtual ~Texture2DArray();
        
        virtual Vector2i getSize() const = 0;
        
        virtual int getCount() const = 0;
        
        virtual void setData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area, int count) = 0;
        
        virtual void getData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) const = 0;
    };
}

#endif
