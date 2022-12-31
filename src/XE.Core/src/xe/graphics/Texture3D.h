
#ifndef __XE_GRAPHICS_TEXTURE3D_HPP__
#define __XE_GRAPHICS_TEXTURE3D_HPP__

#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    class Texture3D : public Texture {
    public:
        virtual TextureType getType() const override;

    public:
        virtual ~Texture3D();
        
        virtual Vector3i getSize() const = 0;
        
        virtual void setData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) = 0;
        
        virtual void getData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) const = 0;
    };    
}

#endif
