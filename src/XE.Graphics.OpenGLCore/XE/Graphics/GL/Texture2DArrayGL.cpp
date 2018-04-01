
#include "Texture2DArrayGL.hpp"
#include "Conversion.hpp"

namespace XE::Graphics::GL {
    Texture2DArrayGL::Texture2DArrayGL(const PixelFormat format, const XE::Math::Vector2i &size, const int count) 
            : TextureBaseGL(GL_TEXTURE_2D_ARRAY) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = ConvertToGL(m_format);
        const GLenum formatGL = ConvertToGL(format);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, formatGL, size.X, size.Y, count);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    Texture2DArrayGL::~Texture2DArrayGL() {}

    void Texture2DArrayGL::SetData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Math::Recti &area, int count) {
        const XE::Math::Vector2i offset = area.MinEdge;
        const XE::Math::Vector2i size = area.ComputeSize();
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipLevel, offset.X, offset.Y, arrayIndex, size.X, size.Y, count, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
        
    void Texture2DArrayGL::GetData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Math::Recti &area) const {
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glGetTexImage(GL_TEXTURE_2D_ARRAY, mipLevel, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
}
