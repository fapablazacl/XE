
#ifndef __XE_DATATYPE_HPP__
#define __XE_DATATYPE_HPP__

#include <cstdint>

namespace xe {
    //! Bitfield-packed encoding of element size, numeric kind, and dimensionality.
    using TypeEncoding = uint16_t;

    /**
     * @brief Element byte size field for a TypeEncoding value.
     */
    enum class TypeSize : uint8_t {
        Byte1,
        Byte2,
        Byte4,
        Byte8,
    };

    /**
     * @brief Base numeric kind field for a TypeEncoding value.
     */
    enum class TypeKind : uint8_t {
        UInt,
        Int,
        Float,
    };

    /**
     * @brief Dimensionality classification of a TypeEncoding value.
     */
    enum class TypeShape : uint8_t {
        //! Single element (cols=1, rows=0)
        Scalar,
        //! 1D array of 2–4 elements (cols=2-4, rows=0)
        Vector,
        //! 2D array (cols=2-4, rows=2-4)
        Matrix,
    };

    //! Bit layout: bytes[1:0] | kind[3:2] | cols[6:4] | rows[9:7]
    constexpr uint16_t bytesShift = 0u;
    constexpr uint16_t bytesMask  = 0x3u;
    constexpr uint16_t typeShift  = 2u;
    constexpr uint16_t typeMask   = 0x3u;
    constexpr uint16_t colsShift  = 4u;
    constexpr uint16_t colsMask   = 0x7u;
    constexpr uint16_t rowsShift  = 7u;
    constexpr uint16_t rowsMask   = 0x7u;

    constexpr TypeEncoding makeScalarType(TypeSize bytes, TypeKind type) {
        return static_cast<TypeEncoding>(
            (static_cast<uint16_t>(bytes) & bytesMask) |
            ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
            (1u << colsShift)
        );
    }

    constexpr TypeEncoding makeVectorType(TypeSize bytes, TypeKind type, uint8_t dim) {
        return static_cast<TypeEncoding>(
            (static_cast<uint16_t>(bytes) & bytesMask) |
            ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
            ((static_cast<uint16_t>(dim) & colsMask) << colsShift)
        );
    }

    constexpr TypeEncoding makeMatrixType(TypeSize bytes, TypeKind type, uint8_t cols, uint8_t rows) {
        return static_cast<TypeEncoding>(
            (static_cast<uint16_t>(bytes) & bytesMask) |
            ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
            ((static_cast<uint16_t>(cols) & colsMask) << colsShift) |
            ((static_cast<uint16_t>(rows) & rowsMask) << rowsShift)
        );
    }

    /**
     * @brief Returns the size in bytes of a single element for the given encoded type.
     */
    constexpr uint8_t getTypeSizeInBytes(TypeEncoding dt) {
        return static_cast<uint8_t>(1u << (dt & bytesMask));
    }

    /**
     * @brief Returns the numeric kind (UInt, Int, or Float) encoded in dt.
     */
    constexpr TypeKind getTypeKind(TypeEncoding dt) {
        return static_cast<TypeKind>((dt >> typeShift) & typeMask);
    }

    /**
     * @brief Returns the column count encoded in dt.
     */
    constexpr uint8_t getTypeCols(TypeEncoding dt) {
        return static_cast<uint8_t>((dt >> colsShift) & colsMask);
    }

    /**
     * @brief Returns the row count encoded in dt.
     */
    constexpr uint8_t getTypeRows(TypeEncoding dt) {
        return static_cast<uint8_t>((dt >> rowsShift) & rowsMask);
    }

    /**
     * @brief Returns the shape (Scalar, Vector, or Matrix) of the encoded type.
     */
    constexpr TypeShape getTypeShape(TypeEncoding dt) {
        if (getTypeRows(dt) != 0u) return TypeShape::Matrix;
        if (getTypeCols(dt) > 1u)  return TypeShape::Vector;
        return TypeShape::Scalar;
    }

    // scalars
    constexpr TypeEncoding Scalar_UInt8   = makeScalarType(TypeSize::Byte1, TypeKind::UInt);
    constexpr TypeEncoding Scalar_UInt16  = makeScalarType(TypeSize::Byte2, TypeKind::UInt);
    constexpr TypeEncoding Scalar_UInt32  = makeScalarType(TypeSize::Byte4, TypeKind::UInt);
    constexpr TypeEncoding Scalar_UInt64  = makeScalarType(TypeSize::Byte8, TypeKind::UInt);
    constexpr TypeEncoding Scalar_Int8    = makeScalarType(TypeSize::Byte1, TypeKind::Int);
    constexpr TypeEncoding Scalar_Int16   = makeScalarType(TypeSize::Byte2, TypeKind::Int);
    constexpr TypeEncoding Scalar_Int32   = makeScalarType(TypeSize::Byte4, TypeKind::Int);
    constexpr TypeEncoding Scalar_Int64   = makeScalarType(TypeSize::Byte8, TypeKind::Int);
    constexpr TypeEncoding Scalar_Float16 = makeScalarType(TypeSize::Byte2, TypeKind::Float);
    constexpr TypeEncoding Scalar_Float32 = makeScalarType(TypeSize::Byte4, TypeKind::Float);
    constexpr TypeEncoding Scalar_Float64 = makeScalarType(TypeSize::Byte8, TypeKind::Float);

    // vectors
    constexpr TypeEncoding Vec2_UInt8   = makeVectorType(TypeSize::Byte1, TypeKind::UInt,  2);
    constexpr TypeEncoding Vec3_UInt8   = makeVectorType(TypeSize::Byte1, TypeKind::UInt,  3);
    constexpr TypeEncoding Vec4_UInt8   = makeVectorType(TypeSize::Byte1, TypeKind::UInt,  4);
    constexpr TypeEncoding Vec2_UInt16  = makeVectorType(TypeSize::Byte2, TypeKind::UInt,  2);
    constexpr TypeEncoding Vec3_UInt16  = makeVectorType(TypeSize::Byte2, TypeKind::UInt,  3);
    constexpr TypeEncoding Vec4_UInt16  = makeVectorType(TypeSize::Byte2, TypeKind::UInt,  4);
    constexpr TypeEncoding Vec2_UInt32  = makeVectorType(TypeSize::Byte4, TypeKind::UInt,  2);
    constexpr TypeEncoding Vec3_UInt32  = makeVectorType(TypeSize::Byte4, TypeKind::UInt,  3);
    constexpr TypeEncoding Vec4_UInt32  = makeVectorType(TypeSize::Byte4, TypeKind::UInt,  4);
    constexpr TypeEncoding Vec2_Int8    = makeVectorType(TypeSize::Byte1, TypeKind::Int,   2);
    constexpr TypeEncoding Vec3_Int8    = makeVectorType(TypeSize::Byte1, TypeKind::Int,   3);
    constexpr TypeEncoding Vec4_Int8    = makeVectorType(TypeSize::Byte1, TypeKind::Int,   4);
    constexpr TypeEncoding Vec2_Int16   = makeVectorType(TypeSize::Byte2, TypeKind::Int,   2);
    constexpr TypeEncoding Vec3_Int16   = makeVectorType(TypeSize::Byte2, TypeKind::Int,   3);
    constexpr TypeEncoding Vec4_Int16   = makeVectorType(TypeSize::Byte2, TypeKind::Int,   4);
    constexpr TypeEncoding Vec2_Int32   = makeVectorType(TypeSize::Byte4, TypeKind::Int,   2);
    constexpr TypeEncoding Vec3_Int32   = makeVectorType(TypeSize::Byte4, TypeKind::Int,   3);
    constexpr TypeEncoding Vec4_Int32   = makeVectorType(TypeSize::Byte4, TypeKind::Int,   4);
    constexpr TypeEncoding Vec2_Float16 = makeVectorType(TypeSize::Byte2, TypeKind::Float, 2);
    constexpr TypeEncoding Vec3_Float16 = makeVectorType(TypeSize::Byte2, TypeKind::Float, 3);
    constexpr TypeEncoding Vec4_Float16 = makeVectorType(TypeSize::Byte2, TypeKind::Float, 4);
    constexpr TypeEncoding Vec2_Float32 = makeVectorType(TypeSize::Byte4, TypeKind::Float, 2);
    constexpr TypeEncoding Vec3_Float32 = makeVectorType(TypeSize::Byte4, TypeKind::Float, 3);
    constexpr TypeEncoding Vec4_Float32 = makeVectorType(TypeSize::Byte4, TypeKind::Float, 4);
    constexpr TypeEncoding Vec2_Float64 = makeVectorType(TypeSize::Byte8, TypeKind::Float, 2);
    constexpr TypeEncoding Vec3_Float64 = makeVectorType(TypeSize::Byte8, TypeKind::Float, 3);
    constexpr TypeEncoding Vec4_Float64 = makeVectorType(TypeSize::Byte8, TypeKind::Float, 4);

    // matrices (square)
    constexpr TypeEncoding Mat2_Float32 = makeMatrixType(TypeSize::Byte4, TypeKind::Float, 2, 2);
    constexpr TypeEncoding Mat3_Float32 = makeMatrixType(TypeSize::Byte4, TypeKind::Float, 3, 3);
    constexpr TypeEncoding Mat4_Float32 = makeMatrixType(TypeSize::Byte4, TypeKind::Float, 4, 4);
    constexpr TypeEncoding Mat2_Float64 = makeMatrixType(TypeSize::Byte8, TypeKind::Float, 2, 2);
    constexpr TypeEncoding Mat3_Float64 = makeMatrixType(TypeSize::Byte8, TypeKind::Float, 3, 3);
    constexpr TypeEncoding Mat4_Float64 = makeMatrixType(TypeSize::Byte8, TypeKind::Float, 4, 4);

    // matrices (non-square)
    constexpr TypeEncoding Mat3x4_Float32 = makeMatrixType(TypeSize::Byte4, TypeKind::Float, 3, 4);
    constexpr TypeEncoding Mat4x3_Float32 = makeMatrixType(TypeSize::Byte4, TypeKind::Float, 4, 3);

    enum class
    [[deprecated]]
    DataType : std::int16_t { MetaFirst, Unknown = MetaFirst, UInt8, UInt16, UInt32, UInt64, Int8, Int16, Int32, Int64, Float16, Float32, Float64, MetaCount };

    [[deprecated]]
    extern int bytesize(const DataType dataType);
} // namespace xe

#endif
