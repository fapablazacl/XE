
#ifndef __XE_GRAPHICS_GL_CONVERSION_HPP__
#define XE_GRAPHICS_GL_CONVERSION_HPP_

#include "gl.h"
#include <cstdint>

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
    enum class TextureCubeMapSide;
} // namespace XE

namespace XE {
    extern GLenum convertToGL(DataType type);
    extern GLenum convertToGL(PixelFormat format);
    extern GLenum convertToGL(BufferType type);
    extern GLenum convertToGL(BufferUsage usage, BufferAccess access);
    extern GLenum convertToGL(PrimitiveType type);
    extern GLenum convertToGL(DepthFunc func);
    extern GLenum convertToGL(PolygonMode mode);
    extern GLenum convertToGL(FrontFaceOrder order);
    extern GLenum convertToGL(BlendParam param);
    extern GLenum convertToGL(TextureFilter filter);
    extern GLenum convertToGL(TextureWrap wrap);
    extern GLenum convertToGL(TextureCubeMapSide side);
    extern GLboolean convertToGL(bool value);
} // namespace XE

#endif
