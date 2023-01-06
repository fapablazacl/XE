
#include <xe/graphics/gl/TextureCubeMapGL.h>
#include <xe/graphics/gl/Conversion.h>

namespace XE {
    TextureCubeMapGL::TextureCubeMapGL(const PixelFormat format, const Vector2i &size, const PixelFormat sourceFormat, const DataType sourceDataType, const std::array<TextureCubeMapSide, 6> &sourceDataSides, const void **sourceData) : TextureBaseGL(GL_TEXTURE_CUBE_MAP) {
        
        m_size = size;
        m_format = format;

        const GLenum internalFormatGL = convertToGL(m_format);
        const GLenum formatGL = convertToGL(sourceFormat);
        const GLenum typeGL = convertToGL(sourceDataType);

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
        
        for (size_t i = 0; i < sourceDataSides.size(); i++) {
            const GLenum side = convertToGL(sourceDataSides[i]);
            glTexImage2D(side, 0, internalFormatGL, m_size.X, m_size.Y, 0, formatGL, typeGL, sourceData[i]);
        }
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    
    TextureCubeMapGL::~TextureCubeMapGL() {}

    
    void TextureCubeMapGL::setData(const void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMapSide, const DataType surfaceDataType, const PixelFormat surfaceFormat, const Recti &area) {
        
        const Vector2i offset = area.getMinEdge();
        const Vector2i size = area.getSize();
        const GLenum sideGL = convertToGL(cubeMapSide);
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
        glTexSubImage2D(sideGL, mipLevel, offset.X, offset.Y, size.X, size.Y, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    
    
    void TextureCubeMapGL::getData(void *surfaceData, const int mipLevel, TextureCubeMapSide cubeMapSide, const DataType surfaceDataType, const PixelFormat surfaceFormat, const Recti &) const {        
        const GLenum sideGL = convertToGL(cubeMapSide);
        const GLenum formatGL = convertToGL(surfaceFormat);
        const GLenum dataTypeGL = convertToGL(surfaceDataType);

        glBindTexture(GL_TEXTURE_2D, m_id);
        glGetTexImage(sideGL, mipLevel, formatGL, dataTypeGL, surfaceData);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
