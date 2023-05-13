
#ifndef __XE_GRAPHICS_TEXTURECUBEMAP_HPP__
#define __XE_GRAPHICS_TEXTURECUBEMAP_HPP__

#include <xe/DataType.h>
#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    class TextureCubeMap : public Texture {
    public:
        virtual ~TextureCubeMap();

        TextureType getType() const override { return TextureType::TexCubeMap; }

        virtual Vector2i getSize() const = 0;

        virtual void setData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const DataType surfaceDataType, const PixelFormat surfaceFormat,
                             const Recti &area) = 0;

        virtual void getData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const DataType surfaceDataType, const PixelFormat surfaceFormat,
                             const Recti &area) const = 0;
    };
} // namespace XE

#endif
