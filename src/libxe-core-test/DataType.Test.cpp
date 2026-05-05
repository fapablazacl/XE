#include <catch2/catch_test_macros.hpp>

#include <xe/DataType.h>

using namespace xe;

TEST_CASE("bytesize returns 1 for 1-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(getTypeSizeInBytes(Scalar_UInt8) == 1);
    REQUIRE(getTypeSizeInBytes(Scalar_Int8)  == 1);
}

TEST_CASE("bytesize returns 2 for 2-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(getTypeSizeInBytes(Scalar_UInt16)  == 2);
    REQUIRE(getTypeSizeInBytes(Scalar_Int16)   == 2);
    REQUIRE(getTypeSizeInBytes(Scalar_Float16) == 2);
}

TEST_CASE("bytesize returns 4 for 4-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(getTypeSizeInBytes(Scalar_UInt32)  == 4);
    REQUIRE(getTypeSizeInBytes(Scalar_Int32)   == 4);
    REQUIRE(getTypeSizeInBytes(Scalar_Float32) == 4);
}

TEST_CASE("bytesize returns 8 for 8-byte scalar types", "[datatype][bytesize]") {
    REQUIRE(getTypeSizeInBytes(Scalar_UInt64)  == 8);
    REQUIRE(getTypeSizeInBytes(Scalar_Int64)   == 8);
    REQUIRE(getTypeSizeInBytes(Scalar_Float64) == 8);
}

TEST_CASE("bytesize is consistent across scalars vectors and matrices of same base type", "[datatype][bytesize]") {
    REQUIRE(getTypeSizeInBytes(Vec2_Float32) == getTypeSizeInBytes(Scalar_Float32));
    REQUIRE(getTypeSizeInBytes(Vec3_Float32) == getTypeSizeInBytes(Scalar_Float32));
    REQUIRE(getTypeSizeInBytes(Vec4_Float32) == getTypeSizeInBytes(Scalar_Float32));
    REQUIRE(getTypeSizeInBytes(Mat4_Float32) == getTypeSizeInBytes(Scalar_Float32));

    REQUIRE(getTypeSizeInBytes(Vec4_UInt8)   == getTypeSizeInBytes(Scalar_UInt8));
    REQUIRE(getTypeSizeInBytes(Vec4_Int16)   == getTypeSizeInBytes(Scalar_Int16));
    REQUIRE(getTypeSizeInBytes(Mat4_Float64) == getTypeSizeInBytes(Scalar_Float64));
}

TEST_CASE("kind returns UInt for unsigned integer types", "[datatype][kind]") {
    REQUIRE(getTypeKind(Scalar_UInt8)  == TypeKind::UInt);
    REQUIRE(getTypeKind(Scalar_UInt16) == TypeKind::UInt);
    REQUIRE(getTypeKind(Scalar_UInt32) == TypeKind::UInt);
    REQUIRE(getTypeKind(Scalar_UInt64) == TypeKind::UInt);
}

TEST_CASE("kind returns Int for signed integer types", "[datatype][kind]") {
    REQUIRE(getTypeKind(Scalar_Int8)  == TypeKind::Int);
    REQUIRE(getTypeKind(Scalar_Int16) == TypeKind::Int);
    REQUIRE(getTypeKind(Scalar_Int32) == TypeKind::Int);
    REQUIRE(getTypeKind(Scalar_Int64) == TypeKind::Int);
}

TEST_CASE("kind returns Float for floating-point types", "[datatype][kind]") {
    REQUIRE(getTypeKind(Scalar_Float16) == TypeKind::Float);
    REQUIRE(getTypeKind(Scalar_Float32) == TypeKind::Float);
    REQUIRE(getTypeKind(Scalar_Float64) == TypeKind::Float);
}

TEST_CASE("kind is preserved in vector and matrix variants", "[datatype][kind]") {
    REQUIRE(getTypeKind(Vec4_Float32)   == TypeKind::Float);
    REQUIRE(getTypeKind(Vec4_Int32)     == TypeKind::Int);
    REQUIRE(getTypeKind(Vec4_UInt8)     == TypeKind::UInt);
    REQUIRE(getTypeKind(Mat4_Float32)   == TypeKind::Float);
    REQUIRE(getTypeKind(Mat4_Float64)   == TypeKind::Float);
    REQUIRE(getTypeKind(Mat3x4_Float32) == TypeKind::Float);
}

TEST_CASE("cols returns 1 for all scalar types", "[datatype][cols]") {
    REQUIRE(getTypeCols(Scalar_UInt8)   == 1);
    REQUIRE(getTypeCols(Scalar_Int32)   == 1);
    REQUIRE(getTypeCols(Scalar_Float32) == 1);
    REQUIRE(getTypeCols(Scalar_Float64) == 1);
}

TEST_CASE("cols returns correct dimension for vector types", "[datatype][cols]") {
    REQUIRE(getTypeCols(Vec2_Float32) == 2);
    REQUIRE(getTypeCols(Vec3_Float32) == 3);
    REQUIRE(getTypeCols(Vec4_Float32) == 4);
    REQUIRE(getTypeCols(Vec2_UInt8)   == 2);
    REQUIRE(getTypeCols(Vec3_Int16)   == 3);
    REQUIRE(getTypeCols(Vec4_UInt8)   == 4);
}

TEST_CASE("cols returns correct column count for matrix types", "[datatype][cols]") {
    REQUIRE(getTypeCols(Mat2_Float32)   == 2);
    REQUIRE(getTypeCols(Mat3_Float32)   == 3);
    REQUIRE(getTypeCols(Mat4_Float32)   == 4);
    REQUIRE(getTypeCols(Mat2_Float64)   == 2);
    REQUIRE(getTypeCols(Mat4_Float64)   == 4);
    REQUIRE(getTypeCols(Mat3x4_Float32) == 3);
    REQUIRE(getTypeCols(Mat4x3_Float32) == 4);
}

TEST_CASE("rows returns 0 for all scalar types", "[datatype][rows]") {
    REQUIRE(getTypeRows(Scalar_UInt8)   == 0);
    REQUIRE(getTypeRows(Scalar_Int32)   == 0);
    REQUIRE(getTypeRows(Scalar_Float32) == 0);
    REQUIRE(getTypeRows(Scalar_Float64) == 0);
}

TEST_CASE("rows returns 0 for all vector types", "[datatype][rows]") {
    REQUIRE(getTypeRows(Vec2_Float32) == 0);
    REQUIRE(getTypeRows(Vec3_Float32) == 0);
    REQUIRE(getTypeRows(Vec4_Float32) == 0);
    REQUIRE(getTypeRows(Vec4_UInt8)   == 0);
    REQUIRE(getTypeRows(Vec4_Int32)   == 0);
    REQUIRE(getTypeRows(Vec4_Float64) == 0);
}

TEST_CASE("rows returns correct row count for matrix types", "[datatype][rows]") {
    REQUIRE(getTypeRows(Mat2_Float32)   == 2);
    REQUIRE(getTypeRows(Mat3_Float32)   == 3);
    REQUIRE(getTypeRows(Mat4_Float32)   == 4);
    REQUIRE(getTypeRows(Mat2_Float64)   == 2);
    REQUIRE(getTypeRows(Mat4_Float64)   == 4);
    REQUIRE(getTypeRows(Mat3x4_Float32) == 4);
    REQUIRE(getTypeRows(Mat4x3_Float32) == 3);
}

TEST_CASE("all TypeEncoding functions are usable in constexpr context", "[datatype][constexpr]") {
    static_assert(getTypeSizeInBytes(Scalar_Float32) == 4);
    static_assert(getTypeSizeInBytes(Scalar_UInt8)   == 1);
    static_assert(getTypeSizeInBytes(Vec4_Float32)   == 4);
    static_assert(getTypeSizeInBytes(Mat4_Float64)   == 8);

    static_assert(getTypeKind(Scalar_Float32) == TypeKind::Float);
    static_assert(getTypeKind(Scalar_Int32)   == TypeKind::Int);
    static_assert(getTypeKind(Scalar_UInt8)   == TypeKind::UInt);

    static_assert(getTypeCols(Scalar_Float32)  == 1);
    static_assert(getTypeCols(Vec4_Float32)    == 4);
    static_assert(getTypeCols(Mat4_Float32)    == 4);
    static_assert(getTypeCols(Mat3x4_Float32)  == 3);

    static_assert(getTypeRows(Scalar_Float32)  == 0);
    static_assert(getTypeRows(Vec4_Float32)    == 0);
    static_assert(getTypeRows(Mat4_Float32)    == 4);
    static_assert(getTypeRows(Mat3x4_Float32)  == 4);
    static_assert(getTypeRows(Mat4x3_Float32)  == 3);

    static_assert(getTypeShape(Scalar_Float32) == TypeShape::Scalar);
    static_assert(getTypeShape(Vec4_Float32)   == TypeShape::Vector);
    static_assert(getTypeShape(Mat4_Float32)   == TypeShape::Matrix);

    REQUIRE(true); // suppress empty-test-case warning
}

TEST_CASE("shape returns Scalar for all scalar types", "[datatype][shape]") {
    REQUIRE(getTypeShape(Scalar_UInt8)   == TypeShape::Scalar);
    REQUIRE(getTypeShape(Scalar_Int32)   == TypeShape::Scalar);
    REQUIRE(getTypeShape(Scalar_Float32) == TypeShape::Scalar);
    REQUIRE(getTypeShape(Scalar_Float64) == TypeShape::Scalar);
}

TEST_CASE("shape returns Vector for all vector types", "[datatype][shape]") {
    REQUIRE(getTypeShape(Vec2_Float32) == TypeShape::Vector);
    REQUIRE(getTypeShape(Vec3_Float32) == TypeShape::Vector);
    REQUIRE(getTypeShape(Vec4_Float32) == TypeShape::Vector);
    REQUIRE(getTypeShape(Vec4_UInt8)   == TypeShape::Vector);
    REQUIRE(getTypeShape(Vec4_Int32)   == TypeShape::Vector);
    REQUIRE(getTypeShape(Vec2_Float64) == TypeShape::Vector);
}

TEST_CASE("shape returns Matrix for all matrix types", "[datatype][shape]") {
    REQUIRE(getTypeShape(Mat2_Float32)   == TypeShape::Matrix);
    REQUIRE(getTypeShape(Mat3_Float32)   == TypeShape::Matrix);
    REQUIRE(getTypeShape(Mat4_Float32)   == TypeShape::Matrix);
    REQUIRE(getTypeShape(Mat4_Float64)   == TypeShape::Matrix);
    REQUIRE(getTypeShape(Mat3x4_Float32) == TypeShape::Matrix);
    REQUIRE(getTypeShape(Mat4x3_Float32) == TypeShape::Matrix);
}
