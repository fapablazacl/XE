
#ifndef __XE_GRAPHICS_ES2_CONVERSION_HPP__
#define __XE_GRAPHICS_ES2_CONVERSION_HPP__

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
    extern GLenum convertToES(const DataType type);
    extern GLenum convertToES(const PixelFormat format);
    extern GLenum convertToES(const BufferType type);
    extern GLenum convertToES(const BufferUsage usage, const BufferAccess access);
    extern GLenum convertToES(const PrimitiveType type);
    extern GLenum convertToES(const DepthFunc func);
    extern GLenum convertToES(const PolygonMode mode);
    extern GLenum convertToES(const FrontFaceOrder order);
    extern GLenum convertToES(const BlendParam param);
    extern GLenum convertToES(const TextureFilter filter);
    extern GLenum convertToES(const TextureWrap wrap);
    extern GLboolean convertToES(const bool value);
}

#endif 
