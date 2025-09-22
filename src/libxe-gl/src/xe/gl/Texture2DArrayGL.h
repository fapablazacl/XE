
#ifndef __XE_GRAPHICS_GL_TEXTURE2DARRAYGL_HPP__
#define XE_GRAPHICS_GL_TEXTURE2DARRAYGL_HPP_

#include <xe/graphics/Texture2DArray.h>

#include "TextureBaseGL.h"

namespace XE {
    class Texture2DArrayGL : public Texture2DArray, public TextureBaseGL {
    public:
        Texture2DArrayGL(PixelFormat format, const Vector2i &size, int count);

        ~Texture2DArrayGL() override;

        PixelFormat getFormat() const override {
            return m_format;
        }

    
        Vector2i getSize() const override {
            return m_size;
        }

        int getCount() const override {
            return m_count;
        }

        void setData(
            const void *surfaceData, int mipLevel, int arrayIndex, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area, int count
        ) override;

        void
        getData(void *surfaceData, int mipLevel, int arrayIndex, PixelFormat surfaceFormat, DataType surfaceDataType, const Recti &area) const override;

    private:
        PixelFormat m_format;
        Vector2i m_size = {0, 0};
        int m_count = 0;
    };
} // namespace XE

#endif
