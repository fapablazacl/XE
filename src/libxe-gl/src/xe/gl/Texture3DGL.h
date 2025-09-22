
#ifndef __XE_GRAPHICS_GL_TEXTURE3DGL_HPP__
#define XE_GRAPHICS_GL_TEXTURE3DGL_HPP_

#include <xe/graphics/Texture3D.h>

#include "TextureBaseGL.h"

namespace XE {
    class Texture3DGL : public Texture3D, public TextureBaseGL {
    public:
        Texture3DGL(PixelFormat format, const Vector3i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData);

        ~Texture3DGL() override;

        PixelFormat getFormat() const override {
            return m_format;
        }

    
        Vector3i getSize() const override {
            return m_size;
        }

        void setData(const void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Boxi &volume) override;

        void getData(void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Boxi &volume) const override;

    private:
        PixelFormat m_format;
        Vector3i m_size;
    };
} // namespace XE

#endif
