
#ifndef __XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP__

#include <xe/graphics/TextureCubeMap.h>

#include <array>
#include "TextureBaseGL.h"

namespace XE {
    class TextureCubeMapGL : public TextureCubeMap, public TextureBaseGL {
    public:
        TextureCubeMapGL(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const std::array<TextureCubeMapSide, 6> &sourceDataSides, const void **sourceData);
        
        virtual ~TextureCubeMapGL();

        PixelFormat getFormat() const override {
            return m_format;
        }
        
        Vector2i getSize() const override {
            return m_size;
        }
        
        void setData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const DataType surfaceDataType, const PixelFormat surfaceFormat, const Recti &area) override;
        
        void getData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const DataType surfaceDataType, const PixelFormat surfaceFormat, const Recti &area) const override;
    private:
        PixelFormat m_format;
        Vector2i m_size;
    };
}

#endif
