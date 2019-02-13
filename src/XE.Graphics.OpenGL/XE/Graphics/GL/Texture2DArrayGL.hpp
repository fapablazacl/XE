
#ifndef __XE_GRAPHICS_GL_TEXTURE2DARRAYGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURE2DARRAYGL_HPP__

#include <XE/Graphics/Texture2DArray.hpp>

#include "TextureBaseGL.hpp"

namespace XE {
    class Texture2DArrayGL : public Texture2DArray, public TextureBaseGL {
    public:
        Texture2DArrayGL(const PixelFormat format, const XE::Vector2i &size, const int count);

        virtual ~Texture2DArrayGL();

        virtual PixelFormat GetFormat() const override {
            return m_format;
        }
        
    public:
        virtual XE::Vector2i GetSize() const override {
            return m_size;
        }

        virtual int GetCount() const override {
            return m_count;
        }
        
        virtual void SetData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Recti &area, int count) override;
        
        virtual void GetData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Recti &area) const override;

    private:
        PixelFormat m_format;
        XE::Vector2i m_size;
        int m_count;
    };
}

#endif
