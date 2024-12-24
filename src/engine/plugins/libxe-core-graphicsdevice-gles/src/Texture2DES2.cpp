
#include <xe/graphics/gles2/ConversionES.h>
#include <xe/graphics/gles2/Texture2DES2.h>
#include <xe/graphics/gles2/Util.h>

namespace XE {
    Texture2DES::Texture2DES(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData)
        : TextureBaseES(GL_TEXTURE_2D) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = convertToES(m_format);
        const GLenum formatGL = convertToES(sourceFormat);
        const GLenum typeGL = convertToES(sourceDataType);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormatGL, m_size.X, m_size.Y, 0, formatGL, typeGL, sourceData);
        glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    Texture2DES::~Texture2DES() {}

    void Texture2DES::setData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) {
        const Vector2i offset = area.getMinEdge();
        const Vector2i size = area.getSize();
        const GLenum formatGL = convertToES(surfaceFormat);
        const GLenum dataTypeGL = convertToES(surfaceDataType);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexSubImage2D(GL_TEXTURE_2D, mipLevel, offset.X, offset.Y, size.X, size.Y, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_2D, 0);

        XE_GRAPHICS_GL_CHECK_ERROR();
    }

    void Texture2DES::getData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType) const {}
} // namespace XE
