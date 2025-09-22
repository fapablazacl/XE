
#ifndef __XE_GRAPHICS_GL_TEXTURE2DGL_HPP__
#define XE_GRAPHICS_GL_TEXTURE2DGL_HPP_

#include <xe/graphics/Texture2D.h>

#include "TextureBaseGL.h"

namespace XE {
    class Texture2DGL : public Texture2D, public TextureBaseGL {
    public:
        Texture2DGL(PixelFormat format, const Vector2i &size, PixelFormat sourceFormat, DataType sourceDataType, const void *sourceData);

        ~Texture2DGL() override;

        PixelFormat getFormat() const override {
            return m_format;
        }

    
        Vector2i getSize() const override {
            return m_size;
        }

        void setData(const void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area) override;

        void getData(void *surfaceData, int mipLevel, PixelFormat surfaceFormat, DataType surfaceDataType) const override;

    private:
        PixelFormat m_format;
        Vector2i m_size;
    };
} // namespace XE

#endif
