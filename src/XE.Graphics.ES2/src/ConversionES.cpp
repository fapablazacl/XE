
#include <XE/Graphics/ES2/ConversionES.h>

#include <cassert>
#include <XE/DataType.h>
#include <XE/Graphics/PixelFormat.h>
#include <XE/Graphics/BufferDescriptor.h>
#include <XE/Graphics/Subset.h>
#include <XE/Graphics/Material.h>

namespace XE {
    GLenum convertToES(const DataType type) {
        switch (type) {
            case DataType::UInt8: return GL_UNSIGNED_BYTE;
            case DataType::UInt16: return GL_UNSIGNED_SHORT;
            case DataType::UInt32: return GL_UNSIGNED_INT;
            case DataType::Int8: return GL_BYTE;
            case DataType::Int16: return GL_SHORT;
            case DataType::Int32: return GL_INT;
            case DataType::Float32: return GL_FLOAT;
            default: 
                assert(false);
                return 0;
        }
    }

    GLenum convertToES(const PixelFormat format) {
        switch (format) {
            case PixelFormat::R8G8B8: return GL_RGB;
            case PixelFormat::R8G8B8A8: return GL_RGBA;
            default: 
                assert(false);
                return 0;
        }
    }

    GLenum convertToES(const BufferType type) {
        switch (type) {
        case BufferType::Vertex: return GL_ARRAY_BUFFER;
        case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
        default: 
            assert(false);
            return 0;
        }
    }

    GLenum convertToES(const BufferUsage, const BufferAccess access) {
        switch (access) {
        case BufferAccess::Dynamic:
            return GL_DYNAMIC_DRAW;

        case BufferAccess::Static:
            return GL_STATIC_DRAW;
            
        case BufferAccess::Stream:
            return GL_STREAM_DRAW;

        default:
            assert(false);
            return 0;
        }
    }

    GLenum convertToES(const PrimitiveType type) {
        switch (type) {
            case PrimitiveType::PointList: return GL_POINTS;
            case PrimitiveType::LineList: return GL_LINES;
            case PrimitiveType::LineStrip: return GL_LINE_STRIP;
            case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
            case PrimitiveType::TriangleList: return GL_TRIANGLES;
            case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
            default: 
                assert(false);
                return 0;
        }
    }

    GLenum convertToES(const DepthFunc func) {
        switch (func) {
            case DepthFunc::Never:          return GL_NEVER;
            case DepthFunc::Less:           return GL_LESS;
            case DepthFunc::Equal:          return GL_EQUAL;
            case DepthFunc::LesserEqual:    return GL_LEQUAL;
            case DepthFunc::Greater:        return GL_GREATER;
            case DepthFunc::NotEqual:       return GL_NOTEQUAL;
            case DepthFunc::GreaterEqual:   return GL_GEQUAL;
            case DepthFunc::Always:         return GL_ALWAYS;
            default: 
                assert(false);
                return 0;
        }
    }


    GLenum convertToES(const FrontFaceOrder order) {
        switch (order) {
            case FrontFaceOrder::CounterClockwise: return GL_CCW;
            case FrontFaceOrder::Clockwise: return GL_CW;
            default: assert(false); return 0;
        }
    }

    GLenum convertToES(const BlendParam param) {
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
            default:    
                assert(false);
                return 0;
        }
    }

    GLenum convertToES(const TextureFilter filter) {
        switch (filter) {
            case TextureFilter::Linear: return GL_LINEAR;
            case TextureFilter::Nearest: return GL_NEAREST;
            default: 
                assert(false);
                return 0;
        }
    }

    GLenum convertToES(const TextureWrap wrap) {
        switch (wrap) {
            case TextureWrap::Repeat: return GL_REPEAT;
            default: 
                assert(false);
                return 0;
        }
    }

    GLboolean convertToES(const bool value) {
        return value ? GL_TRUE : GL_FALSE;
    }
}
