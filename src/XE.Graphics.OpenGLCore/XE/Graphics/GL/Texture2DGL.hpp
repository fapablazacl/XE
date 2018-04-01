
#ifndef __XE_GRAPHICS_GL_TEXTURE2DGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURE2DGL_HPP__

#include <XE/Graphics/Texture2D.hpp>
#include <glad/glad.h>

namespace XE::Graphics::GL {
    class Texture2DGL : public Texture2D {
    public:
        Texture2DGL(const PixelFormat format, const XE::Math::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData);

        virtual ~Texture2DGL();

        virtual PixelFormat GetFormat() const override {
            return m_format;
        }
        
    public:
        virtual XE::Math::Vector2i GetSize() const override {
            return m_size;
        }
        
        virtual void SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Math::Recti &area) override;

        virtual void GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const override;

    public:
        GLuint GetID() const {
            return m_id;
        }

        GLenum GetTarget() const {
            return m_target;
        }

    private:
        GLuint m_id;
        GLenum m_target;

        PixelFormat m_format;
        XE::Math::Vector2i m_size;
    };
}

#endif
