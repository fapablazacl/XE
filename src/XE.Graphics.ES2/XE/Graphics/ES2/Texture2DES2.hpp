
#ifndef __XE_GRAPHICS_GL_TEXTURE2DGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURE2DGL_HPP__

#include <XE/Graphics/Texture2D.hpp>

#include "TextureBaseES2.hpp"

namespace XE {
    class Texture2DGL : public Texture2D, public TextureBaseGL {
    public:
        Texture2DGL(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData);

        virtual ~Texture2DGL();

        virtual PixelFormat getFormat() const override {
            return m_format;
        }
        
    public:
        virtual Vector2i getSize() const override {
            return m_size;
        }
        
        virtual void setData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) override;

        virtual void getData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const override;

    private:
        PixelFormat m_format;
        Vector2i m_size;
    };
}

#endif
