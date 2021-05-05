
#ifndef __XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURECUBEMAPGL_HPP__

#include <XE/Graphics/TextureCubeMap.h>

#include "TextureBaseGL.h"

namespace XE {
    /*
    class TextureCubeMapGL : public TextureCubeMap, public TextureBaseGL {
    public:
        TextureCubeMapGL(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData);

        virtual ~TextureCubeMapGL();

        virtual PixelFormat GetFormat() const override {
            return m_format;
        }
        
    public:
        virtual Vector2i GetSize() const = 0;
        
        virtual void SetData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const Recti &area) = 0;
        
        virtual void GetData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMap, const PixelFormat surfaceFormat, const Recti &area) const = 0;

    private:
        PixelFormat m_format;
        Vector3i m_size;
    };
    */
}

#endif
