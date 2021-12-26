
#include <catch2/catch_all.hpp>

#include <XE/DataType.h>
#include <XE/Graphics/PixelFormat.h>
#include <XE/Graphics/BufferDescriptor.h>
#include <XE/Graphics/Subset.h>
#include <XE/Graphics/Material.h>

#include <XE/Graphics/GL/Conversion.h>


TEST_CASE("OpenGL Conversion Function Enumerations") {
    SECTION("GLenum XE::convertToGL(const DataType type)") {
        REQUIRE(XE::convertToGL(XE::DataType::UInt8) == GL_UNSIGNED_BYTE);
        REQUIRE(XE::convertToGL(XE::DataType::UInt16) == GL_UNSIGNED_SHORT);
        REQUIRE(XE::convertToGL(XE::DataType::UInt32) == GL_UNSIGNED_INT);
        REQUIRE(XE::convertToGL(XE::DataType::Int8) == GL_BYTE);
        REQUIRE(XE::convertToGL(XE::DataType::Int16) == GL_SHORT);
        REQUIRE(XE::convertToGL(XE::DataType::Int32) == GL_INT);
        REQUIRE(XE::convertToGL(XE::DataType::Float32) == GL_FLOAT);
        REQUIRE(XE::convertToGL(XE::DataType::Float64) == GL_DOUBLE);
    }

    SECTION("GLenum XE::convertToGL(const PixelFormat format)") {
        REQUIRE(XE::convertToGL(XE::PixelFormat::R8G8B8) == GL_RGB);
        REQUIRE(XE::convertToGL(XE::PixelFormat::R8G8B8A8) == GL_RGBA);
    }

    SECTION("GLenum XE::convertToGL(const BufferType type)") {
        REQUIRE(XE::convertToGL(XE::BufferType::Vertex) == GL_ARRAY_BUFFER);
        REQUIRE(XE::convertToGL(XE::BufferType::Index) == GL_ELEMENT_ARRAY_BUFFER);
    }

    SECTION("GLenum XE::convertToGL(const BufferUsage usage, const BufferAccess access)") {
        REQUIRE(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Dynamic) == GL_DYNAMIC_COPY);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Dynamic) == GL_DYNAMIC_READ);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Dynamic) == GL_DYNAMIC_DRAW);

        REQUIRE(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Static) == GL_STATIC_COPY);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Static) == GL_STATIC_READ);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Static) == GL_STATIC_DRAW);

        REQUIRE(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Stream) == GL_STREAM_COPY);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Stream) == GL_STREAM_READ);
        REQUIRE(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Stream) == GL_STREAM_DRAW);
    }

    SECTION("GLenum XE::convertToGL(const PrimitiveType type)") {
        REQUIRE(XE::convertToGL(XE::PrimitiveType::PointList) == GL_POINTS);
        REQUIRE(XE::convertToGL(XE::PrimitiveType::LineList) == GL_LINES);
        REQUIRE(XE::convertToGL(XE::PrimitiveType::LineStrip) == GL_LINE_STRIP);
        REQUIRE(XE::convertToGL(XE::PrimitiveType::TriangleStrip) == GL_TRIANGLE_STRIP);
        REQUIRE(XE::convertToGL(XE::PrimitiveType::TriangleList) == GL_TRIANGLES);
        REQUIRE(XE::convertToGL(XE::PrimitiveType::TriangleFan) == GL_TRIANGLE_FAN);
    }

    SECTION("GLenum XE::convertToGL(const DepthFunc func)") {
        REQUIRE(XE::convertToGL(XE::DepthFunc::Never) == GL_NEVER);
        REQUIRE(XE::convertToGL(XE::DepthFunc::Less) == GL_LESS);
        REQUIRE(XE::convertToGL(XE::DepthFunc::Equal) == GL_EQUAL);
        REQUIRE(XE::convertToGL(XE::DepthFunc::LesserEqual) == GL_LEQUAL);
        REQUIRE(XE::convertToGL(XE::DepthFunc::Greater) == GL_GREATER);
        REQUIRE(XE::convertToGL(XE::DepthFunc::NotEqual) == GL_NOTEQUAL);
        REQUIRE(XE::convertToGL(XE::DepthFunc::GreaterEqual) == GL_GEQUAL);
        REQUIRE(XE::convertToGL(XE::DepthFunc::Always) == GL_ALWAYS);
    }

    SECTION("GLenum XE::convertToGL(const PolygonMode mode)") {
        REQUIRE(XE::convertToGL(XE::PolygonMode::Fill) == GL_FILL);
        REQUIRE(XE::convertToGL(XE::PolygonMode::Line) == GL_LINE);
        REQUIRE(XE::convertToGL(XE::PolygonMode::Point) == GL_POINT);
    }

    SECTION("GLenum XE::convertToGL(const FrontFaceOrder order)") {
        REQUIRE(XE::convertToGL(XE::FrontFaceOrder::CounterClockwise) == GL_CCW);
        REQUIRE(XE::convertToGL(XE::FrontFaceOrder::Clockwise) == GL_CW);
    }

    
    SECTION("GLenum XE::convertToGL(const BlendParam param)") {
        REQUIRE(XE::convertToGL(XE::BlendParam::Zero) == GL_ZERO);
        REQUIRE(XE::convertToGL(XE::BlendParam::One) == GL_ONE);
        REQUIRE(XE::convertToGL(XE::BlendParam::SourceColor) == GL_SRC_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusSourceColor) == GL_ONE_MINUS_SRC_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::DestinationColor) == GL_DST_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusDestinationColor) == GL_ONE_MINUS_DST_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::SourceAlpha) == GL_SRC_ALPHA);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusSourceAlpha) == GL_ONE_MINUS_SRC_ALPHA);
        REQUIRE(XE::convertToGL(XE::BlendParam::DestinationAlpha) == GL_DST_ALPHA);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusDestinationAlpha) == GL_ONE_MINUS_DST_ALPHA);
        REQUIRE(XE::convertToGL(XE::BlendParam::ConstantColor) == GL_CONSTANT_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusConstantColor) == GL_ONE_MINUS_CONSTANT_COLOR);
        REQUIRE(XE::convertToGL(XE::BlendParam::ConstantAlpha) == GL_CONSTANT_ALPHA);
        REQUIRE(XE::convertToGL(XE::BlendParam::OneMinusConstantAlpha) == GL_ONE_MINUS_CONSTANT_ALPHA);
    }

    SECTION("GLenum XE::convertToGL(const TextureFilter filter)") {
        REQUIRE(XE::convertToGL(XE::TextureFilter::Linear) == GL_LINEAR);
        REQUIRE(XE::convertToGL(XE::TextureFilter::Nearest) == GL_NEAREST);
    }

    SECTION("GLenum XE::convertToGL(const TextureWrap wrap)") {
        REQUIRE(XE::convertToGL(XE::TextureWrap::Clamp) == GL_CLAMP_TO_BORDER);
        REQUIRE(XE::convertToGL(XE::TextureWrap::Repeat) == GL_REPEAT);
    }
    
    SECTION("GLenum XE::convertToGL(const bool value)") {
        REQUIRE(XE::convertToGL(true) == GL_TRUE);
        REQUIRE(XE::convertToGL(false) == GL_FALSE);
    }
}
