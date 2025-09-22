
#ifndef __XE_GRAPHICS_TEXTURE3D_HPP__
#define XE_GRAPHICS_TEXTURE3D_HPP_

#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    class Texture3D : public Texture {
    public:
        TextureType getType() const override;

    
        ~Texture3D() override;

        virtual Vector3i getSize() const = 0;

        virtual void setData(const void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Boxi &volume) = 0;

        virtual void getData(void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Boxi &volume) const = 0;
    };
} // namespace XE

#endif
