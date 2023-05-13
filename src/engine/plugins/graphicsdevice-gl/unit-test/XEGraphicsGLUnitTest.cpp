
#include <gtest/gtest.h>

#include <xe/DataType.h>
#include <xe/graphics/BufferDescriptor.h>
#include <xe/graphics/Material.h>
#include <xe/graphics/PixelFormat.h>
#include <xe/graphics/Subset.h>

#include <xe/graphics/gl/Conversion.h>

TEST(OpenGLConversionTest, DataType) {
    EXPECT_EQ(XE::convertToGL(XE::DataType::UInt8), GL_UNSIGNED_BYTE);
    EXPECT_EQ(XE::convertToGL(XE::DataType::UInt16), GL_UNSIGNED_SHORT);
    EXPECT_EQ(XE::convertToGL(XE::DataType::UInt32), GL_UNSIGNED_INT);
    EXPECT_EQ(XE::convertToGL(XE::DataType::Int8), GL_BYTE);
    EXPECT_EQ(XE::convertToGL(XE::DataType::Int16), GL_SHORT);
    EXPECT_EQ(XE::convertToGL(XE::DataType::Int32), GL_INT);
    EXPECT_EQ(XE::convertToGL(XE::DataType::Float32), GL_FLOAT);
    EXPECT_EQ(XE::convertToGL(XE::DataType::Float64), GL_DOUBLE);
}

TEST(OpenGLConversionTest, PixelFormat) {
    EXPECT_EQ(XE::convertToGL(XE::PixelFormat::R8G8B8), GL_RGB);
    EXPECT_EQ(XE::convertToGL(XE::PixelFormat::R8G8B8A8), GL_RGBA);
}

TEST(OpenGLConversionTest, BufferType) {
    EXPECT_EQ(XE::convertToGL(XE::BufferType::Vertex), GL_ARRAY_BUFFER);
    EXPECT_EQ(XE::convertToGL(XE::BufferType::Index), GL_ELEMENT_ARRAY_BUFFER);
}

TEST(OpenGLConversionTest, BufferAccess) {
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Dynamic), GL_DYNAMIC_COPY);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Dynamic), GL_DYNAMIC_READ);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Dynamic), GL_DYNAMIC_DRAW);

    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Static), GL_STATIC_COPY);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Static), GL_STATIC_READ);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Static), GL_STATIC_DRAW);

    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Copy, XE::BufferAccess::Stream), GL_STREAM_COPY);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Read, XE::BufferAccess::Stream), GL_STREAM_READ);
    EXPECT_EQ(XE::convertToGL(XE::BufferUsage::Write, XE::BufferAccess::Stream), GL_STREAM_DRAW);
}

TEST(OpenGLConversionTest, PrimitiveType) {
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::PointList), GL_POINTS);
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::LineList), GL_LINES);
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::LineStrip), GL_LINE_STRIP);
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::TriangleStrip), GL_TRIANGLE_STRIP);
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::TriangleList), GL_TRIANGLES);
    EXPECT_EQ(XE::convertToGL(XE::PrimitiveType::TriangleFan), GL_TRIANGLE_FAN);
}

TEST(OpenGLConversionTest, DepthFunc) {
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::Never), GL_NEVER);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::Less), GL_LESS);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::Equal), GL_EQUAL);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::LesserEqual), GL_LEQUAL);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::Greater), GL_GREATER);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::NotEqual), GL_NOTEQUAL);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::GreaterEqual), GL_GEQUAL);
    EXPECT_EQ(XE::convertToGL(XE::DepthFunc::Always), GL_ALWAYS);
}

TEST(OpenGLConversionTest, PolygonMode) {
    EXPECT_EQ(XE::convertToGL(XE::PolygonMode::Fill), GL_FILL);
    EXPECT_EQ(XE::convertToGL(XE::PolygonMode::Line), GL_LINE);
    EXPECT_EQ(XE::convertToGL(XE::PolygonMode::Point), GL_POINT);
}

TEST(OpenGLConversionTest, FrontFaceOrder) {
    EXPECT_EQ(XE::convertToGL(XE::FrontFaceOrder::CounterClockwise), GL_CCW);
    EXPECT_EQ(XE::convertToGL(XE::FrontFaceOrder::Clockwise), GL_CW);
}

TEST(OpenGLConversionTest, BlendParam) {
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::Zero), GL_ZERO);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::One), GL_ONE);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::SourceColor), GL_SRC_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusSourceColor), GL_ONE_MINUS_SRC_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::DestinationColor), GL_DST_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusDestinationColor), GL_ONE_MINUS_DST_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::SourceAlpha), GL_SRC_ALPHA);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusSourceAlpha), GL_ONE_MINUS_SRC_ALPHA);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::DestinationAlpha), GL_DST_ALPHA);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusDestinationAlpha), GL_ONE_MINUS_DST_ALPHA);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::ConstantColor), GL_CONSTANT_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusConstantColor), GL_ONE_MINUS_CONSTANT_COLOR);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::ConstantAlpha), GL_CONSTANT_ALPHA);
    EXPECT_EQ(XE::convertToGL(XE::BlendParam::OneMinusConstantAlpha), GL_ONE_MINUS_CONSTANT_ALPHA);
}

TEST(OpenGLConversionTest, TextureFilter) {
    EXPECT_EQ(XE::convertToGL(XE::TextureFilter::Linear), GL_LINEAR);
    EXPECT_EQ(XE::convertToGL(XE::TextureFilter::Nearest), GL_NEAREST);
}

TEST(OpenGLConversionTest, TextureWrap) {
    EXPECT_EQ(XE::convertToGL(XE::TextureWrap::Clamp), GL_CLAMP_TO_BORDER);
    EXPECT_EQ(XE::convertToGL(XE::TextureWrap::Repeat), GL_REPEAT);
}

TEST(OpenGLConversionTest, StandardBool) {
    EXPECT_EQ(XE::convertToGL(true), GL_TRUE);
    EXPECT_EQ(XE::convertToGL(false), GL_FALSE);
}
