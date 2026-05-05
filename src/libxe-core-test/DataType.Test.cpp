#include <catch2/catch_test_macros.hpp>

#include <xe/DataType.h>

using namespace xe;

TEST_CASE("bytesize returns 1 for 1-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(bytesize(Scalar_UInt8) == 1);
    REQUIRE(bytesize(Scalar_Int8)  == 1);
}

TEST_CASE("bytesize returns 2 for 2-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(bytesize(Scalar_UInt16)  == 2);
    REQUIRE(bytesize(Scalar_Int16)   == 2);
    REQUIRE(bytesize(Scalar_Float16) == 2);
}

TEST_CASE("bytesize returns 4 for 4-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(bytesize(Scalar_UInt32)  == 4);
    REQUIRE(bytesize(Scalar_Int32)   == 4);
    REQUIRE(bytesize(Scalar_Float32) == 4);
}

TEST_CASE("bytesize returns 8 for 8-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(bytesize(Scalar_UInt64)  == 8);
    REQUIRE(bytesize(Scalar_Int64)   == 8);
    REQUIRE(bytesize(Scalar_Float64) == 8);
}

TEST_CASE("bytesize is consistent across scalars vectors and matrices of same base type", "[datatype][bytesize]") {
    REQUIRE(bytesize(Vec2_Float32) == bytesize(Scalar_Float32));
    REQUIRE(bytesize(Vec3_Float32) == bytesize(Scalar_Float32));
    REQUIRE(bytesize(Vec4_Float32) == bytesize(Scalar_Float32));
    REQUIRE(bytesize(Mat4_Float32) == bytesize(Scalar_Float32));

    REQUIRE(bytesize(Vec4_UInt8)   == bytesize(Scalar_UInt8));
    REQUIRE(bytesize(Vec4_Int16)   == bytesize(Scalar_Int16));
    REQUIRE(bytesize(Mat4_Float64) == bytesize(Scalar_Float64));
}

TEST_CASE("kind returns UInt for unsigned integer types", "[datatype][kind]") {
    REQUIRE(kind(Scalar_UInt8)  == TypeKind::UInt);
    REQUIRE(kind(Scalar_UInt16) == TypeKind::UInt);
    REQUIRE(kind(Scalar_UInt32) == TypeKind::UInt);
    REQUIRE(kind(Scalar_UInt64) == TypeKind::UInt);
}

TEST_CASE("kind returns Int for signed integer types", "[datatype][kind]") {
    REQUIRE(kind(Scalar_Int8)  == TypeKind::Int);
    REQUIRE(kind(Scalar_Int16) == TypeKind::Int);
    REQUIRE(kind(Scalar_Int32) == TypeKind::Int);
    REQUIRE(kind(Scalar_Int64) == TypeKind::Int);
}

TEST_CASE("kind returns Float for floating-point types", "[datatype][kind]") {
    REQUIRE(kind(Scalar_Float16) == TypeKind::Float);
    REQUIRE(kind(Scalar_Float32) == TypeKind::Float);
    REQUIRE(kind(Scalar_Float64) == TypeKind::Float);
}

TEST_CASE("kind is preserved in vector and matrix variants", "[datatype][kind]") {
    REQUIRE(kind(Vec4_Float32)   == TypeKind::Float);
    REQUIRE(kind(Vec4_Int32)     == TypeKind::Int);
    REQUIRE(kind(Vec4_UInt8)     == TypeKind::UInt);
    REQUIRE(kind(Mat4_Float32)   == TypeKind::Float);
    REQUIRE(kind(Mat4_Float64)   == TypeKind::Float);
    REQUIRE(kind(Mat3x4_Float32) == TypeKind::Float);
}

TEST_CASE("cols returns 1 for all scalar types", "[datatype][cols]") {
    REQUIRE(cols(Scalar_UInt8)   == 1);
    REQUIRE(cols(Scalar_Int32)   == 1);
    REQUIRE(cols(Scalar_Float32) == 1);
    REQUIRE(cols(Scalar_Float64) == 1);
}

TEST_CASE("cols returns correct dimension for vector types", "[datatype][cols]") {
    REQUIRE(cols(Vec2_Float32) == 2);
    REQUIRE(cols(Vec3_Float32) == 3);
    REQUIRE(cols(Vec4_Float32) == 4);
    REQUIRE(cols(Vec2_UInt8)   == 2);
    REQUIRE(cols(Vec3_Int16)   == 3);
    REQUIRE(cols(Vec4_UInt8)   == 4);
}

TEST_CASE("cols returns correct column count for matrix types", "[datatype][cols]") {
    REQUIRE(cols(Mat2_Float32)   == 2);
    REQUIRE(cols(Mat3_Float32)   == 3);
    REQUIRE(cols(Mat4_Float32)   == 4);
    REQUIRE(cols(Mat2_Float64)   == 2);
    REQUIRE(cols(Mat4_Float64)   == 4);
    REQUIRE(cols(Mat3x4_Float32) == 3);
    REQUIRE(cols(Mat4x3_Float32) == 4);
}

TEST_CASE("rows returns 0 for all scalar types", "[datatype][rows]") {
    REQUIRE(rows(Scalar_UInt8)   == 0);
    REQUIRE(rows(Scalar_Int32)   == 0);
    REQUIRE(rows(Scalar_Float32) == 0);
    REQUIRE(rows(Scalar_Float64) == 0);
}

TEST_CASE("rows returns 0 for all vector types", "[datatype][rows]") {
    REQUIRE(rows(Vec2_Float32) == 0);
    REQUIRE(rows(Vec3_Float32) == 0);
    REQUIRE(rows(Vec4_Float32) == 0);
    REQUIRE(rows(Vec4_UInt8)   == 0);
    REQUIRE(rows(Vec4_Int32)   == 0);
    REQUIRE(rows(Vec4_Float64) == 0);
}

TEST_CASE("rows returns correct row count for matrix types", "[datatype][rows]") {
    REQUIRE(rows(Mat2_Float32)   == 2);
    REQUIRE(rows(Mat3_Float32)   == 3);
    REQUIRE(rows(Mat4_Float32)   == 4);
    REQUIRE(rows(Mat2_Float64)   == 2);
    REQUIRE(rows(Mat4_Float64)   == 4);
    REQUIRE(rows(Mat3x4_Float32) == 4);
    REQUIRE(rows(Mat4x3_Float32) == 3);
}

TEST_CASE("all TypeEncoding functions are usable in constexpr context", "[datatype][constexpr]") {
    static_assert(bytesize(Scalar_Float32) == 4);
    static_assert(bytesize(Scalar_UInt8)   == 1);
    static_assert(bytesize(Vec4_Float32)   == 4);
    static_assert(bytesize(Mat4_Float64)   == 8);

    static_assert(kind(Scalar_Float32) == TypeKind::Float);
    static_assert(kind(Scalar_Int32)   == TypeKind::Int);
    static_assert(kind(Scalar_UInt8)   == TypeKind::UInt);

    static_assert(cols(Scalar_Float32)  == 1);
    static_assert(cols(Vec4_Float32)    == 4);
    static_assert(cols(Mat4_Float32)    == 4);
    static_assert(cols(Mat3x4_Float32)  == 3);

    static_assert(rows(Scalar_Float32)  == 0);
    static_assert(rows(Vec4_Float32)    == 0);
    static_assert(rows(Mat4_Float32)    == 4);
    static_assert(rows(Mat3x4_Float32)  == 4);
    static_assert(rows(Mat4x3_Float32)  == 3);

    static_assert(shape(Scalar_Float32) == TypeShape::Scalar);
    static_assert(shape(Vec4_Float32)   == TypeShape::Vector);
    static_assert(shape(Mat4_Float32)   == TypeShape::Matrix);

    REQUIRE(true); // suppress empty-test-case warning
}

TEST_CASE("shape returns Scalar for all scalar types", "[datatype][shape]") {
    REQUIRE(shape(Scalar_UInt8)   == TypeShape::Scalar);
    REQUIRE(shape(Scalar_Int32)   == TypeShape::Scalar);
    REQUIRE(shape(Scalar_Float32) == TypeShape::Scalar);
    REQUIRE(shape(Scalar_Float64) == TypeShape::Scalar);
}

TEST_CASE("shape returns Vector for all vector types", "[datatype][shape]") {
    REQUIRE(shape(Vec2_Float32) == TypeShape::Vector);
    REQUIRE(shape(Vec3_Float32) == TypeShape::Vector);
    REQUIRE(shape(Vec4_Float32) == TypeShape::Vector);
    REQUIRE(shape(Vec4_UInt8)   == TypeShape::Vector);
    REQUIRE(shape(Vec4_Int32)   == TypeShape::Vector);
    REQUIRE(shape(Vec2_Float64) == TypeShape::Vector);
}

TEST_CASE("shape returns Matrix for all matrix types", "[datatype][shape]") {
    REQUIRE(shape(Mat2_Float32)   == TypeShape::Matrix);
    REQUIRE(shape(Mat3_Float32)   == TypeShape::Matrix);
    REQUIRE(shape(Mat4_Float32)   == TypeShape::Matrix);
    REQUIRE(shape(Mat4_Float64)   == TypeShape::Matrix);
    REQUIRE(shape(Mat3x4_Float32) == TypeShape::Matrix);
    REQUIRE(shape(Mat4x3_Float32) == TypeShape::Matrix);
}
