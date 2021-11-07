
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <XE/DataType.h>
#include <XE/Graphics/PixelFormat.h>
#include <XE/Graphics/BufferDescriptor.h>
#include <XE/Graphics/Subset.h>
#include <XE/Graphics/Material.h>

#include <XE/Graphics/GL/Conversion.h>


TEST_CASE("OpenGL Conversion Function Enumerations") {
    SECTION("GLenum convertToGL(const DataType type)") {
        REQUIRE(convertToGL(XE::DataType::UInt8) == GL_UNSIGNED_BYTE);
        REQUIRE(convertToGL(XE::DataType::UInt16) == GL_UNSIGNED_SHORT);
        REQUIRE(convertToGL(XE::DataType::UInt32) == GL_UNSIGNED_INT);
        REQUIRE(convertToGL(XE::DataType::Int8) == GL_BYTE);
        REQUIRE(convertToGL(XE::DataType::Int16) == GL_SHORT);
        REQUIRE(convertToGL(XE::DataType::Int32) == GL_INT);
        REQUIRE(convertToGL(XE::DataType::Float32) == GL_FLOAT);
        REQUIRE(convertToGL(XE::DataType::Float64) == GL_DOUBLE);
    }

    SECTION("GLenum convertToGL(const PixelFormat format)") {
        REQUIRE(convertToGL(XE::PixelFormat::R8G8B8) == GL_RGB);
        REQUIRE(convertToGL(XE::PixelFormat::R8G8B8A8) == GL_RGBA);
    }

    SECTION("GLenum convertToGL(const BufferType type)") {
        REQUIRE(convertToGL(XE::BufferType::Vertex) == GL_ARRAY_BUFFER);
        REQUIRE(convertToGL(XE::BufferType::Index) == GL_ELEMENT_ARRAY_BUFFER);
    }

    SECTION("GLenum convertToGL(const BufferUsage usage, const BufferAccess access)") {
        REQUIRE(convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Dynamic) == GL_DYNAMIC_COPY);
        REQUIRE(convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Dynamic) == GL_DYNAMIC_READ);
        REQUIRE(convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Dynamic) == GL_DYNAMIC_DRAW);

        REQUIRE(convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Static) == GL_STATIC_COPY);
        REQUIRE(convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Static) == GL_STATIC_READ);
        REQUIRE(convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Static) == GL_STATIC_DRAW);

        REQUIRE(convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Stream) == GL_STREAM_COPY);
        REQUIRE(convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Stream) == GL_STREAM_READ);
        REQUIRE(convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Stream) == GL_STREAM_DRAW);
    }

    SECTION("GLenum convertToGL(const PrimitiveType type)") {
        REQUIRE(convertToGL(XE::PrimitiveType::PointList) == GL_POINTS);
        REQUIRE(convertToGL(XE::PrimitiveType::LineList) == GL_LINES);
        REQUIRE(convertToGL(XE::PrimitiveType::LineStrip) == GL_LINE_STRIP);
        REQUIRE(convertToGL(XE::PrimitiveType::TriangleStrip) == GL_TRIANGLE_STRIP);
        REQUIRE(convertToGL(XE::PrimitiveType::TriangleList) == GL_TRIANGLES);
        REQUIRE(convertToGL(XE::PrimitiveType::TriangleFan) == GL_TRIANGLE_FAN);
    }

    SECTION("GLenum convertToGL(const DepthFunc func)") {
        REQUIRE(convertToGL(XE::DepthFunc::Never) == GL_NEVER);
        REQUIRE(convertToGL(XE::DepthFunc::Less) == GL_LESS);
        REQUIRE(convertToGL(XE::DepthFunc::Equal) == GL_EQUAL);
        REQUIRE(convertToGL(XE::DepthFunc::LesserEqual) == GL_LEQUAL);
        REQUIRE(convertToGL(XE::DepthFunc::Greater) == GL_GREATER);
        REQUIRE(convertToGL(XE::DepthFunc::NotEqual) == GL_NOTEQUAL);
        REQUIRE(convertToGL(XE::DepthFunc::GreaterEqual) == GL_GEQUAL);
        REQUIRE(convertToGL(XE::DepthFunc::Always) == GL_ALWAYS);
    }

    SECTION("GLenum convertToGL(const PolygonMode mode)") {
        REQUIRE(convertToGL(XE::PolygonMode::Fill) == GL_FILL);
        REQUIRE(convertToGL(XE::PolygonMode::Line) == GL_LINE);
        REQUIRE(convertToGL(XE::PolygonMode::Point) == GL_POINT);
    }

    SECTION("GLenum convertToGL(const FrontFaceOrder order)") {
        REQUIRE(convertToGL(XE::FrontFaceOrder::CounterClockwise) == GL_CCW);
        REQUIRE(convertToGL(XE::FrontFaceOrder::Clockwise) == GL_CW);
    }

    
    SECTION("GLenum convertToGL(const BlendParam param)") {
        REQUIRE(convertToGL(XE::BlendParam::Zero) == GL_ZERO);
        REQUIRE(convertToGL(XE::BlendParam::One) == GL_ONE);
        REQUIRE(convertToGL(XE::BlendParam::SourceColor) == GL_SRC_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusSourceColor) == GL_ONE_MINUS_SRC_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::DestinationColor) == GL_DST_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusDestinationColor) == GL_ONE_MINUS_DST_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::SourceAlpha) == GL_SRC_ALPHA);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusSourceAlpha) == GL_ONE_MINUS_SRC_ALPHA);
        REQUIRE(convertToGL(XE::BlendParam::DestinationAlpha) == GL_DST_ALPHA);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusDestinationAlpha) == GL_ONE_MINUS_DST_ALPHA);
        REQUIRE(convertToGL(XE::BlendParam::ConstantColor) == GL_CONSTANT_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR);
        REQUIRE(convertToGL(XE::BlendParam::ConstantAlpha) == GL_CONSTANT_ALPHA);
        REQUIRE(convertToGL(XE::BlendParam::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA);
    }

    SECTION("GLenum convertToGL(const TextureFilter filter)") {
        REQUIRE(convertToGL(XE::TextureFilter::Linear) == GL_LINEAR);
        REQUIRE(convertToGL(XE::TextureFilter::Nearest) == GL_NEAREST);
    }

    SECTION("GLenum convertToGL(const TextureWrap wrap)") {
        REQUIRE(convertToGL(XE::TextureWrap::Clamp) == GL_CLAMP_TO_BORDER);
        REQUIRE(convertToGL(XE::TextureWrap::Repeat) == GL_REPEAT);
    }
}
