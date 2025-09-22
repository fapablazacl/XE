
#include "Texture2DArrayGL.h"
#include "Conversion.h"
#include "xe/gl/TextureBaseGL.h"
#include "xe/gl/gl.h"
#include "xe/math/Rect.h"
#include "xe/math/Vector.h"

namespace XE {
    Texture2DArrayGL::Texture2DArrayGL(const PixelFormat format, const Vector2i &size, const int count) : TextureBaseGL(GL_TEXTURE_2D_ARRAY), m_format(format), m_size(size) {

        
        

        // const GLenum internalFormatGL = convertToGL(m_format);
        const GLenum formatGL = convertToGL(format);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, formatGL, size.X, size.Y, count);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    Texture2DArrayGL::~Texture2DArrayGL() {
    }

    void Texture2DArrayGL::setData(
        const void *surfaceData, const int mipLevel, const int arrayIndex, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti &area, int count
    ) {
        const Vector2i offset = area.getMinEdge();
        const Vector2i size = area.getSize();
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, mipLevel, offset.X, offset.Y, arrayIndex, size.X, size.Y, count, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }

    void Texture2DArrayGL::getData(void *surfaceData, const int mipLevel, const int /*arrayIndex*/, const PixelFormat surfaceFormat, const DataType surfaceDataType, const Recti & /*area*/) const {
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
        glGetTexImage(GL_TEXTURE_2D_ARRAY, mipLevel, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
} // namespace XE
