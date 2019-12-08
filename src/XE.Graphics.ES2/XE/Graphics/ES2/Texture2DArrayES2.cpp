
#include "Texture2DArrayES2.hpp"
#include "Conversion.hpp"

namespace XE {
    Texture2DArrayGL::Texture2DArrayGL(const PixelFormat format, const Vector2i &size, const int count) 
            : TextureBaseGL(GL_TEXTURE_2D_ARRAY) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = convertToGL(m_format);
        const GLenum formatGL = convertToGL(format);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, formatGL, size.X, size.Y, count);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    Texture2DArrayGL::~Texture2DArrayGL() {}

    void Texture2DArrayGL::setData(const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area, int count) {
        const Vector2i offset = area.minEdge;
        const Vector2i size = area.computeSize();
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipLevel, offset.X, offset.Y, arrayIndex, size.X, size.Y, count, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
        
    void Texture2DArrayGL::getData(void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area) const {
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        ::glGetTexImage(GL_TEXTURE_2D_ARRAY, mipLevel, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
}
