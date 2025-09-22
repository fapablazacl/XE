
#ifndef __XE_GRAPHICS_TEXTURECUBEMAP_HPP__
#define XE_GRAPHICS_TEXTURECUBEMAP_HPP_

#include <xe/DataType.h>
#include <xe/Predef.h>
#include <xe/graphics/Texture.h>

namespace XE {
    class TextureCubeMap : public Texture {
    public:
        ~TextureCubeMap() override;

        TextureType getType() const override {
            return TextureType::TexCubeMap;
        }

        virtual Vector2i getSize() const = 0;

        virtual void
        setData(const void *surfaceData, int mipLevel, TextureCubeMapSide cubeMap, DataType surfaceDataType, PixelFormat surfaceFormat, const Recti &area) = 0;

        virtual void
        getData(void *surfaceData, int mipLevel, TextureCubeMapSide cubeMap, DataType surfaceDataType, PixelFormat surfaceFormat, const Recti &area) const = 0;
    };
} // namespace XE

#endif
