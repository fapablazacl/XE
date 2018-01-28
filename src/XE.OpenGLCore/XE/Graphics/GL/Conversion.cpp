
#include "Conversion.hpp"

#include <XE/DataType.hpp>
#include <XE/Graphics/PixelFormat.hpp>

namespace XE::Graphics::GL {
    GLenum ConvertToGL(const DataType type) {
        switch (type) {
            case DataType::UInt8: return GL_UNSIGNED_BYTE;
            case DataType::UInt16: return GL_UNSIGNED_SHORT;
            case DataType::UInt32: return GL_UNSIGNED_INT;
            case DataType::Int8: return GL_BYTE;
            case DataType::Int16: return GL_SHORT;
            case DataType::Int32: return GL_INT;
            case DataType::Float32: return GL_FLOAT;
            case DataType::Float64: return GL_DOUBLE;
            default: return 0;
        }
    }

    GLenum ConvertToGL(const PixelFormat format) {
        switch (format) {
            case PixelFormat::R8G8B8: return GL_RGB;
            case PixelFormat::R8G8B8A8: return GL_RGBA;
            default: return 0;
        }
    }
}
