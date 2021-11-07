
#include <XE/Graphics/GL/Conversion.h>

#include <cassert>
#include <XE/DataType.h>
#include <XE/Graphics/PixelFormat.h>
#include <XE/Graphics/BufferDescriptor.h>
#include <XE/Graphics/Subset.h>
#include <XE/Graphics/Material.h>

namespace XE {
    static const std::array<GLenum, 12> dataType {
        static_cast<GLenum>(0),
        GL_UNSIGNED_BYTE, 
        GL_UNSIGNED_SHORT, 
        GL_UNSIGNED_INT,
        static_cast<GLenum>(0),
        GL_BYTE, 
        GL_SHORT, 
        GL_INT,
        static_cast<GLenum>(0),
        static_cast<GLenum>(0),
        GL_FLOAT, 
        GL_DOUBLE
    };


    GLenum convertToGL(const DataType type) {
        assert(type >= DataType::MetaFirst);
        assert(type < DataType::MetaCount);

        const GLenum result = dataType[static_cast<int>(type)];

        assert(result != static_cast<GLenum>(0));

        return result;
    }


    static const std::array<GLenum, 3> pixelFormat {
        static_cast<GLenum>(0), 
        GL_RGB, 
        GL_RGBA
    };

    GLenum convertToGL(const PixelFormat format) {
        assert(format >= PixelFormat::MetaFirst);
        assert(format < PixelFormat::MetaCount);

        const GLenum result = pixelFormat[static_cast<int>(format)];

        assert(result != static_cast<GLenum>(0));

        return result;
    }


    static const std::array<GLenum, 2> bufferType {
        GL_ARRAY_BUFFER,
        GL_ELEMENT_ARRAY_BUFFER
    };

    GLenum convertToGL(const BufferType type) {
        return bufferType[static_cast<int>(type)];
    }


    static const std::array<std::array<GLenum, 3>, 3> bufferUsageAccess {{
        { GL_STATIC_COPY, GL_STATIC_READ, GL_STATIC_DRAW },
        { GL_DYNAMIC_COPY, GL_DYNAMIC_READ, GL_DYNAMIC_DRAW },
        { GL_STREAM_COPY, GL_STREAM_READ, GL_STREAM_DRAW }
    }};

    GLenum convertToGL(const BufferUsage usage, const BufferAccess access) {
        const auto& bufferUsage = bufferUsageAccess[static_cast<int>(access)];

        return bufferUsage[static_cast<int>(usage)];
    }


    static const std::array<GLenum, 8> primitiveType {
        GL_POINTS,
        GL_LINES, 
        GL_LINE_STRIP,
        GL_TRIANGLE_STRIP, 
        GL_TRIANGLES, 
        GL_TRIANGLE_FAN
    };

    GLenum convertToGL(const PrimitiveType type) {
        return primitiveType[static_cast<int>(type)];
    }
    

    static const std::array<GLenum, 8> depthFunc {
        GL_NEVER, GL_LESS,
        GL_EQUAL, GL_LEQUAL, GL_GREATER,
        GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS
    };

    GLenum convertToGL(const DepthFunc func) {
        return depthFunc[static_cast<int>(func)];
    }


    static const std::array<GLenum, 3> polygonMode {
        GL_POINT, GL_LINE, GL_FILL
    };

    GLenum convertToGL(const PolygonMode mode) {
        return polygonMode[static_cast<int>(mode)];
    }

    
    static const std::array<GLenum, 2> frontFaceOrder {
        GL_CW, GL_CCW
    };

    GLenum convertToGL(const FrontFaceOrder order) {
        return frontFaceOrder[static_cast<int>(order)];
    }

    static const std::array<GLenum, 14> blendParams {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
    };

    GLenum convertToGL(const BlendParam param) {
        return blendParams[static_cast<int>(param)];
    }

    
    static const std::array<GLenum, 2> filters {
        GL_NEAREST, 
        GL_LINEAR
    };

    GLenum convertToGL(const TextureFilter filter) {
        return filters[static_cast<int>(filter)];
    }

    static const std::array<GLenum, 2> textureWraps {
        GL_REPEAT, 
        GL_CLAMP_TO_BORDER
    };
    
    GLenum convertToGL(const TextureWrap wrap) {
        return textureWraps[static_cast<int>(wrap)];
    }
    
    GLboolean convertToGL(const bool value) {
        return value ? GL_TRUE : GL_FALSE;
    }
}
