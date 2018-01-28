
#ifndef __XE_GRAPHICS_GL_CONVERSION_HPP__
#define __XE_GRAPHICS_GL_CONVERSION_HPP__

#include <cstdint>
#include <glad/glad.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE::Graphics {
    enum class PixelFormat;
}

namespace XE::Graphics::GL {
    extern GLenum ConvertToGL(const DataType type);
    extern GLenum ConvertToGL(const PixelFormat format);
}

#endif 
