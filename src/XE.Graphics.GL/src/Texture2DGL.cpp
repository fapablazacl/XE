
#include <XE/Graphics/GL/Texture2DGL.h>
#include <XE/Graphics/GL/Conversion.h>
#include <XE/Graphics/GL/Util.h>

namespace XE {
    Texture2DGL::Texture2DGL(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData)
            : TextureBaseGL(GL_TEXTURE_2D) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = convertToGL(m_format);
        const GLenum formatGL = convertToGL(sourceFormat);
        const GLenum typeGL = convertToGL(sourceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glTexImage2D(GL_TEXTURE_2D, 0, internalFormatGL, m_size.X, m_size.Y, 0, formatGL, typeGL, sourceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    Texture2DGL::~Texture2DGL() {}

    void Texture2DGL::setData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) {
        const Vector2i offset = area.minEdge;
        const Vector2i size = area.computeSize();
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glTexSubImage2D(GL_TEXTURE_2D, mipLevel, offset.X, offset.Y, size.X, size.Y, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
    
    void Texture2DGL::getData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const {
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D, m_id);
        ::glGetTexImage(GL_TEXTURE_2D, mipLevel, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }
}
