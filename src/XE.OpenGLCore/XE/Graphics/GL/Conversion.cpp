
#include "Conversion.hpp"

#include <XE/DataType.hpp>
#include <XE/Graphics/PixelFormat.hpp>
#include <XE/Graphics/BufferAccess.hpp>
#include <XE/Graphics/BufferUsage.hpp>
#include <XE/Graphics/BufferType.hpp>

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

    GLenum ConvertToGL(const BufferType type) {
        switch (type) {
        case BufferType::Vertex: return GL_ARRAY_BUFFER;
        case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
        default: return 0;
        }
    }

    GLenum ConvertToGL(const BufferAccess access, const BufferUsage usage) {
        switch (access) {
        case BufferAccess::Dynamic:
            switch (usage) {
            case BufferUsage::Copy: return GL_DYNAMIC_COPY;
            case BufferUsage::Read: return GL_DYNAMIC_READ;
            case BufferUsage::Write: return GL_DYNAMIC_DRAW;
            default: return 0;
            }

        case BufferAccess::Static:
            switch (usage) {
            case BufferUsage::Copy: return GL_STATIC_COPY;
            case BufferUsage::Read: return GL_STATIC_READ;
            case BufferUsage::Write: return GL_STATIC_DRAW;
            default: return 0;
            }
            
        case BufferAccess::Stream:
            switch (usage) {
            case BufferUsage::Copy: return GL_STREAM_COPY;
            case BufferUsage::Read: return GL_STREAM_READ;
            case BufferUsage::Write: return GL_STREAM_DRAW;
            default: return 0;
            }
        }
    }
}
