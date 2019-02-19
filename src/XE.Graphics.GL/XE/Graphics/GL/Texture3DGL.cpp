
#include "Texture3DGL.hpp"
#include "Conversion.hpp"

namespace XE {
    Texture3DGL::Texture3DGL(const PixelFormat format, const Vector3i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const void *sourceData)
            : TextureBaseGL(GL_TEXTURE_3D) {

        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = ConvertToGL(m_format);
        const GLenum formatGL = ConvertToGL(sourceFormat);
        const GLenum typeGL = ConvertToGL(sourceDataType);

        ::glBindTexture(GL_TEXTURE_3D, m_id);
        ::glTexImage3D(GL_TEXTURE_3D, 0, internalFormatGL, m_size.X, m_size.Y, m_size.Z, 0, formatGL, typeGL, sourceData);
        ::glBindTexture(GL_TEXTURE_3D, 0);
    }

    Texture3DGL::~Texture3DGL() {}

    void Texture3DGL::SetData(const std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) {
        const Vector3i offset = volume.MinEdge;
        const Vector3i size = volume.ComputeSize();
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_3D, m_id);
        ::glTexSubImage3D(GL_TEXTURE_3D, mipLevel, offset.X, offset.Y, offset.Z, size.X, size.Y, size.Z, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_3D, 0);
    }
    
    void Texture3DGL::GetData(std::byte *surfaceData, const int mipLevel, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Boxi &volume) const {
        const GLenum formatGL = ConvertToGL(surfaceFormat);
        const GLenum dataTypeGL = ConvertToGL(surfaceDataType);

        ::glBindTexture(GL_TEXTURE_3D, m_id);
        ::glGetTexImage(GL_TEXTURE_3D, mipLevel, formatGL, dataTypeGL, surfaceData);
        ::glBindTexture(GL_TEXTURE_3D, 0);
    }
}
