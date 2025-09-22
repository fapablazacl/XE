
#ifndef __XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP__
#define XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP_

#include <xe/graphics/TextureCubeMap.h>

#include "TextureBaseGL.h"
#include <array>

namespace XE {
    class TextureCubeMapGL : public TextureCubeMap, public TextureBaseGL {
    public:
        TextureCubeMapGL(
            PixelFormat format,
            const Vector2i &size,
            PixelFormat sourceFormat,
            DataType sourceDataType,
            const std::array<TextureCubeMapSide, 6> &sourceDataSides,
            const void **sourceData
        );

        ~TextureCubeMapGL() override;

        PixelFormat getFormat() const override {
            return m_format;
        }

        Vector2i getSize() const override {
            return m_size;
        }

        void setData(
            const void *surfaceData, int mipLevel, TextureCubeMapSide cubeMap, DataType surfaceDataType, PixelFormat surfaceFormat, const Recti &area
        ) override;

        void getData(
            void *surfaceData, int mipLevel, TextureCubeMapSide cubeMap, DataType surfaceDataType, PixelFormat surfaceFormat, const Recti &area
        ) const override;

    private:
        PixelFormat m_format;
        Vector2i m_size;
    };
} // namespace XE

#endif
