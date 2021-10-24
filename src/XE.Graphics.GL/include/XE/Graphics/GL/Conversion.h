
#ifndef __XE_GRAPHICS_GL_CONVERSION_HPP__
#define __XE_GRAPHICS_GL_CONVERSION_HPP__

#include <cstdint>
#include "glcore.h"

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
    extern GLenum convertToGL(const DataType type);
    extern GLenum convertToGL(const PixelFormat format);
    extern GLenum convertToGL(const BufferType type);
    extern GLenum convertToGL(const BufferUsage usage, const BufferAccess access);
    extern GLenum convertToGL(const PrimitiveType type);
    extern GLenum convertToGL(const DepthFunc func);
    extern GLenum convertToGL(const PolygonMode mode);
    extern GLenum convertToGL(const FrontFaceOrder order);
    extern GLenum convertToGL(const BlendParam param);
    extern GLenum convertToGL(const TextureFilter filter);
    extern GLenum convertToGL(const TextureWrap wrap);
    extern GLboolean convertToGL(const bool value);
}

#endif 
