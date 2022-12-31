
#include <xe/graphics/gl/Texture3DGL.h>
#include <xe/graphics/gl/Conversion.h>

namespace XE {
    Texture3DGL::Texture3DGL(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData) : TextureBaseGL(GL_TEXTURE_3D) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = convertToGL(m_format);
        const GLenum formatGL = convertToGL(sourceFormat);
        const GLenum typeGL = convertToGL(sourceDataType);

        glBindTexture(GL_TEXTURE_3D, m_id);
        glTexImage3D(GL_TEXTURE_3D, 0, internalFormatGL, m_size.X, m_size.Y, m_size.Z, 0, formatGL, typeGL, sourceData);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    Texture3DGL::~Texture3DGL() {}

    void Texture3DGL::setData(const void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) {
        const Vector3i offset = volume.getMinEdge();
        const Vector3i size = volume.getSize();
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        glBindTexture(GL_TEXTURE_3D, m_id);
        glTexSubImage3D(GL_TEXTURE_3D, mipLevel, offset.X, offset.Y, offset.Z, size.X, size.Y, size.Z, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
    
    void Texture3DGL::getData(void *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &) const {
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        glBindTexture(GL_TEXTURE_3D, m_id);
        glGetTexImage(GL_TEXTURE_3D, mipLevel, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_3D, 0);
    }
}
