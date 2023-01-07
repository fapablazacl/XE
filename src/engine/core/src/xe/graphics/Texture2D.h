
#ifndef __XE_GRAPHICS_TEXTURE2D_HPP__
#define __XE_GRAPHICS_TEXTURE2D_HPP__

#include <cstddef>
#include <cstdint>
#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    enum class DataType : std::int16_t;

    class Texture2D : public Texture {
    public:
        virtual TextureType getType() const override;

    public:
        virtual ~Texture2D();

        virtual Vector2i getSize() const = 0;

        virtual void setData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) = 0;

        virtual void getData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const = 0;
    };
} // namespace XE

#endif
