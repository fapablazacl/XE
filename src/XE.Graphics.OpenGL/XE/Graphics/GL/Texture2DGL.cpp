
#include "Texture2DGL.hpp"
#include "Conversion.hpp"
#include "Util.hpp"

namespace XE {
    Texture2DGL::Texture2DGL(const PixelFormat format, const XE::Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData)
            : TextureBaseGL(GL_TEXTURE_2D) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = ConvertToGL(m_format);
        const GLenum formatGL = ConvertToGL(sourceFormat);
        const GLenum typeGL = ConvertToGL(sourceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glTexImage2D(GL_TEXTURE_2D, 0, internalFormatGL, m_size.X, m_size.Y, 0, formatGL, typeGL, sourceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    Texture2DGL::~Texture2DGL() {}

    void Texture2DGL::SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const XE::Recti &area) {
        const XE::Vector2i offset = area.MinEdge;
        const XE::Vector2i size = area.ComputeSize();
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glTexSubImage2D(GL_TEXTURE_2D, mipLevel, offset.X, offset.Y, size.X, size.Y, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
    
    void Texture2DGL::GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const {
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glGetTexImage(GL_TEXTURE_2D, mipLevel, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
}
