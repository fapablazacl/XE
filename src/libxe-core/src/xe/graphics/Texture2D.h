
#ifndef __XE_GRAPHICS_TEXTURE2D_HPP__
#define XE_GRAPHICS_TEXTURE2D_HPP_

#include <cstddef>
#include <cstdint>
#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;

    class Texture2D : public Texture {
    public:
        TextureType getType() const override;

    
        ~Texture2D() override;

        virtual Vector2i getSize() const = 0;

        virtual void setData(const void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area) = 0;

        virtual void getData(void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType) const = 0;
    };
} // namespace XE

#endif
