
#ifndef __XE_GRAPHICS_GL_CONVERSION_HPP__
#define __XE_GRAPHICS_GL_CONVERSION_HPP__

#include <cstdint>
#include <glad/glad.h>

namespace XE {
    enum class DataType : std::int16_t;
}

namespace XE {
    enum class PixelFormat;
    enum class BufferType;
    enum class BufferUsage;
    enum class BufferAccess;
    enum class PrimitiveType;
    enum class DepthFunc;
    enum class PolygonMode;
    enum class FrontFaceOrder;
    enum class BlendParam;
    enum class TextureFilter;
    enum class TextureWrap;
}

namespace XE {
    extern GLenum ConvertToGL(const DataType type);
    extern GLenum ConvertToGL(const PixelFormat format);
    extern GLenum ConvertToGL(const BufferType type);
    extern GLenum ConvertToGL(const BufferUsage usage, const BufferAccess access);
    extern GLenum ConvertToGL(const PrimitiveType type);
    extern GLenum ConvertToGL(const DepthFunc func);
    extern GLenum ConvertToGL(const PolygonMode mode);
    extern GLenum ConvertToGL(const FrontFaceOrder order);
    extern GLenum ConvertToGL(const BlendParam param);
    extern GLenum ConvertToGL(const TextureFilter filter);
    extern GLenum ConvertToGL(const TextureWrap wrap);
}

#endif 
