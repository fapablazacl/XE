
#ifndef __XE_GRAPHICS_GL_TEXTURE3DGL_HPP__
#define __XE_GRAPHICS_GL_TEXTURE3DGL_HPP__

#include <XE/Graphics/Texture3D.hpp>

#include "TextureBaseGL.hpp"

namespace XE {
    class Texture3DGL : public Texture3D, public TextureBaseGL {
    public:
        Texture3DGL(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData);

        virtual ~Texture3DGL();

        virtual PixelFormat GetFormat() const override {
            return m_format;
        }
        
    public:
        virtual Vector3i GetSize() const override {
            return m_size;
        }
        
        virtual void SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) override;
        
        virtual void GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) const override;

    private:
        PixelFormat m_format;
        Vector3i m_size;
    };
}

#endif
