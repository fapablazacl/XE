
#ifndef __XE_GRAPHICS_GL_TEXTURE3DGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURE3DGL_HPP__

#include <xe/graphics/Texture3D.h>

#include "TextureBaseGL.h"

namespace XE {
    class Texture3DGL : public Texture3D, public TextureBaseGL {
    public:
        Texture3DGL(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData);

        virtual ~Texture3DGL();

        virtual PixelFormat getFormat() const override {
            return m_format;
        }
        
    public:
        virtual Vector3i getSize() const override {
            return m_size;
        }
        
        virtual void setData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) override;
        
        virtual void getData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) const override;

    private:
        PixelFormat m_format;
        Vector3i m_size;
    };
}

#endif
