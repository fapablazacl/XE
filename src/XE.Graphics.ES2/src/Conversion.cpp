
#include <XE/Graphics/ES2/Conversion.hpp>

#include <XE/DataType.hpp>
#include <XE/Graphics/PixelFormat.hpp>
#include <XE/Graphics/BufferDescriptor.hpp>
#include <XE/Graphics/Subset.hpp>
#include <XE/Graphics/Material.hpp>

namespace XE {
    GLenum convertToGL(const DataType type) {
        switch (type) {
            case DataType::UInt8: return GL_UNSIGNED_BYTE;
            case DataType::UInt16: return GL_UNSIGNED_SHORT;
            case DataType::UInt32: return GL_UNSIGNED_INT;
            case DataType::Int8: return GL_BYTE;
            case DataType::Int16: return GL_SHORT;
            case DataType::Int32: return GL_INT;
            case DataType::Float32: return GL_FLOAT;
            // case DataType::Float64: return GL_DOUBLE;
            default: return 0;
        }
    }

    GLenum convertToGL(const PixelFormat format) {
        switch (format) {
            case PixelFormat::R8G8B8: return GL_RGB;
            case PixelFormat::R8G8B8A8: return GL_RGBA;
            default: return 0;
        }
    }

    GLenum convertToGL(const BufferType type) {
        switch (type) {
        case BufferType::Vertex: return GL_ARRAY_BUFFER;
        case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
        default: return 0;
        }
    }

    GLenum convertToGL(const BufferUsage usage, const BufferAccess access) {
        switch (access) {
        case BufferAccess::Dynamic:
            switch (usage) {
            // case BufferUsage::Copy: return GL_DYNAMIC_COPY;
            // case BufferUsage::Read: return GL_DYNAMIC_READ;
            case BufferUsage::Write: return GL_DYNAMIC_DRAW;
            default: return 0;
            }

        case BufferAccess::Static:
            switch (usage) {
            // case BufferUsage::Copy: return GL_STATIC_COPY;
            // case BufferUsage::Read: return GL_STATIC_READ;
            case BufferUsage::Write: return GL_STATIC_DRAW;
            default: return 0;
            }
            
        case BufferAccess::Stream:
            switch (usage) {
            // case BufferUsage::Copy: return GL_STREAM_COPY;
            // case BufferUsage::Read: return GL_STREAM_READ;
            case BufferUsage::Write: return GL_STREAM_DRAW;
            default: return 0;
            }

        default:
            return 0;
        }
    }

    GLenum convertToGL(const PrimitiveType type) {
        switch (type) {
            case PrimitiveType::PointList: return GL_POINTS;
            case PrimitiveType::LineList: return GL_LINES;
            case PrimitiveType::LineStrip: return GL_LINE_STRIP;
            case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
            case PrimitiveType::TriangleList: return GL_TRIANGLES;
            case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
            default: return 0;
        }
    }

    GLenum convertToGL(const DepthFunc func) {
        switch (func) {
            case DepthFunc::Never:          return GL_NEVER;
            case DepthFunc::Less:           return GL_LESS;
            case DepthFunc::Equal:          return GL_EQUAL;
            case DepthFunc::LesserEqual:    return GL_LEQUAL;
            case DepthFunc::Greater:        return GL_GREATER;
            case DepthFunc::NotEqual:       return GL_NOTEQUAL;
            case DepthFunc::GreaterEqual:   return GL_GEQUAL;
            case DepthFunc::Always:         return GL_ALWAYS;
            default: return 0;
        }
    }

    GLenum convertToGL(const PolygonMode mode) {
        return 0;
        /*
        switch (mode) {
            case PolygonMode::Fill: return GL_FILL;
            case PolygonMode::Line: return GL_LINE;
            case PolygonMode::Point: return GL_POINT;
            default: return 0;
        }
        */
    }

    GLenum convertToGL(const FrontFaceOrder order) {
        switch (order) {
            case FrontFaceOrder::CounterClockwise: return GL_CCW;
            case FrontFaceOrder::Clockwise: return GL_CW;
            default: return 0;
        }
    }

    GLenum convertToGL(const BlendParam param) {
        switch (param) {
            case BlendParam::Zero: return GL_ZERO;
            case BlendParam::One: return GL_ONE;
            case BlendParam::SourceColor: return GL_SRC_COLOR;
            case BlendParam::OneMinusSourceColor: return GL_ONE_MINUS_SRC_COLOR;
            case BlendParam::DestinationColor: return GL_DST_COLOR;
            case BlendParam::OneMinusDestinationColor: return GL_ONE_MINUS_DST_COLOR;
            case BlendParam::SourceAlpha: return GL_SRC_ALPHA;
            case BlendParam::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
            case BlendParam::DestinationAlpha: return GL_DST_ALPHA;
            case BlendParam::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
            case BlendParam::ConstantColor: return GL_CONSTANT_COLOR;
            case BlendParam::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
            case BlendParam::ConstantAlpha: return GL_CONSTANT_ALPHA;
            case BlendParam::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
            default:    return 0;
        }
    }

    GLenum convertToGL(const TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Linear: return GL_LINEAR;
            case TextureFilter::Nearest: return GL_NEAREST;
            default: return 0;
        }
    }

    GLenum convertToGL(const TextureWrap wrap) {
        switch (wrap) {
            // case TextureWrap::Clamp: return GL_CLAMP_TO_BORDER;
            case TextureWrap::Repeat: return GL_REPEAT;
            default: return 0;
        }
    }
}
