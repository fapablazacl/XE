
#ifndef __XE_DATATYPE_HPP__
#define __XE_DATATYPE_HPP__

#include <cstdint>

namespace xe {
    /**
     * @brief Classifies the dimensionality of an internal::DataType value.
     * Scalars have cols=1 and rows=0; vectors have cols=2-4 and rows=0; matrices have rows=2-4.
     */
    enum class DataTypeShape : uint8_t {
        //! Single element (cols=1, rows=0)
        Scalar,
        //! 1D array of 2–4 elements (cols=2-4, rows=0)
        Vector,
        //! 2D array (cols=2-4, rows=2-4)
        Matrix,
    };

    namespace internal {
        //! bitfield-encoded datatype descriptor
        using DataType = uint16_t;

        enum class DataSize : uint8_t {
            _1,
            _2,
            _4,
            _8,
        };

        enum class DataKind : uint8_t {
            UInt,
            Int,
            Float,
        };

        constexpr uint16_t bytesShift = 0u;
        constexpr uint16_t bytesMask  = 0x3u;
        constexpr uint16_t typeShift  = 2u;
        constexpr uint16_t typeMask   = 0x3u;
        constexpr uint16_t colsShift  = 4u;
        constexpr uint16_t colsMask   = 0x7u;
        constexpr uint16_t rowsShift  = 7u;
        constexpr uint16_t rowsMask   = 0x7u;

        constexpr DataType makeDataTypeScalar(DataSize bytes, DataKind type) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & bytesMask) |
                ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
                (1u << colsShift)
            );
        }

        constexpr DataType makeDataTypeVector(DataSize bytes, DataKind type, uint8_t dim) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & bytesMask) |
                ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
                ((static_cast<uint16_t>(dim) & colsMask) << colsShift)
            );
        }

        constexpr DataType makeDataTypeMatrix(DataSize bytes, DataKind type, uint8_t cols, uint8_t rows) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & bytesMask) |
                ((static_cast<uint16_t>(type) & typeMask) << typeShift) |
                ((static_cast<uint16_t>(cols) & colsMask) << colsShift) |
                ((static_cast<uint16_t>(rows) & rowsMask) << rowsShift)
            );
        }

        /**
         * @brief Returns the byte size of a single element for the given encoded type.
         */
        constexpr uint8_t bytesize(DataType dt) {
            return static_cast<uint8_t>(1u << (dt & bytesMask));
        }

        /**
         * @brief Returns the base numeric type (UINT, INT, or FLOAT) encoded in dt.
         */
        constexpr DataKind baseType(DataType dt) {
            return static_cast<DataKind>((dt >> typeShift) & typeMask);
        }

        /**
         * @brief Returns the column count encoded in dt.
         */
        constexpr uint8_t cols(DataType dt) {
            return static_cast<uint8_t>((dt >> colsShift) & colsMask);
        }

        /**
         * @brief Returns the row count encoded in dt.
         */
        constexpr uint8_t rows(DataType dt) {
            return static_cast<uint8_t>((dt >> rowsShift) & rowsMask);
        }

        /**
         * @brief Returns the shape (Scalar, Vector, or Matrix) of the encoded type.
         */
        constexpr xe::DataTypeShape shape(DataType dt) {
            if (rows(dt) != 0u) return xe::DataTypeShape::Matrix;
            if (cols(dt) > 1u)  return xe::DataTypeShape::Vector;
            return xe::DataTypeShape::Scalar;
        }

        // scalars
        constexpr DataType Scalar_UInt8   = makeDataTypeScalar(DataSize::_1, DataKind::UInt);
        constexpr DataType Scalar_UInt16  = makeDataTypeScalar(DataSize::_2, DataKind::UInt);
        constexpr DataType Scalar_UInt32  = makeDataTypeScalar(DataSize::_4, DataKind::UInt);
        constexpr DataType Scalar_UInt64  = makeDataTypeScalar(DataSize::_8, DataKind::UInt);
        constexpr DataType Scalar_Int8    = makeDataTypeScalar(DataSize::_1, DataKind::Int);
        constexpr DataType Scalar_Int16   = makeDataTypeScalar(DataSize::_2, DataKind::Int);
        constexpr DataType Scalar_Int32   = makeDataTypeScalar(DataSize::_4, DataKind::Int);
        constexpr DataType Scalar_Int64   = makeDataTypeScalar(DataSize::_8, DataKind::Int);
        constexpr DataType Scalar_Float16 = makeDataTypeScalar(DataSize::_2, DataKind::Float);
        constexpr DataType Scalar_Float32 = makeDataTypeScalar(DataSize::_4, DataKind::Float);
        constexpr DataType Scalar_Float64 = makeDataTypeScalar(DataSize::_8, DataKind::Float);

        // vectors
        constexpr DataType Vec2_UInt8   = makeDataTypeVector(DataSize::_1, DataKind::UInt,  2);
        constexpr DataType Vec3_UInt8   = makeDataTypeVector(DataSize::_1, DataKind::UInt,  3);
        constexpr DataType Vec4_UInt8   = makeDataTypeVector(DataSize::_1, DataKind::UInt,  4);
        constexpr DataType Vec2_UInt16  = makeDataTypeVector(DataSize::_2, DataKind::UInt,  2);
        constexpr DataType Vec3_UInt16  = makeDataTypeVector(DataSize::_2, DataKind::UInt,  3);
        constexpr DataType Vec4_UInt16  = makeDataTypeVector(DataSize::_2, DataKind::UInt,  4);
        constexpr DataType Vec2_UInt32  = makeDataTypeVector(DataSize::_4, DataKind::UInt,  2);
        constexpr DataType Vec3_UInt32  = makeDataTypeVector(DataSize::_4, DataKind::UInt,  3);
        constexpr DataType Vec4_UInt32  = makeDataTypeVector(DataSize::_4, DataKind::UInt,  4);
        constexpr DataType Vec2_Int8    = makeDataTypeVector(DataSize::_1, DataKind::Int,   2);
        constexpr DataType Vec3_Int8    = makeDataTypeVector(DataSize::_1, DataKind::Int,   3);
        constexpr DataType Vec4_Int8    = makeDataTypeVector(DataSize::_1, DataKind::Int,   4);
        constexpr DataType Vec2_Int16   = makeDataTypeVector(DataSize::_2, DataKind::Int,   2);
        constexpr DataType Vec3_Int16   = makeDataTypeVector(DataSize::_2, DataKind::Int,   3);
        constexpr DataType Vec4_Int16   = makeDataTypeVector(DataSize::_2, DataKind::Int,   4);
        constexpr DataType Vec2_Int32   = makeDataTypeVector(DataSize::_4, DataKind::Int,   2);
        constexpr DataType Vec3_Int32   = makeDataTypeVector(DataSize::_4, DataKind::Int,   3);
        constexpr DataType Vec4_Int32   = makeDataTypeVector(DataSize::_4, DataKind::Int,   4);
        constexpr DataType Vec2_Float16 = makeDataTypeVector(DataSize::_2, DataKind::Float, 2);
        constexpr DataType Vec3_Float16 = makeDataTypeVector(DataSize::_2, DataKind::Float, 3);
        constexpr DataType Vec4_Float16 = makeDataTypeVector(DataSize::_2, DataKind::Float, 4);
        constexpr DataType Vec2_Float32 = makeDataTypeVector(DataSize::_4, DataKind::Float, 2);
        constexpr DataType Vec3_Float32 = makeDataTypeVector(DataSize::_4, DataKind::Float, 3);
        constexpr DataType Vec4_Float32 = makeDataTypeVector(DataSize::_4, DataKind::Float, 4);
        constexpr DataType Vec2_Float64 = makeDataTypeVector(DataSize::_8, DataKind::Float, 2);
        constexpr DataType Vec3_Float64 = makeDataTypeVector(DataSize::_8, DataKind::Float, 3);
        constexpr DataType Vec4_Float64 = makeDataTypeVector(DataSize::_8, DataKind::Float, 4);

        // matrices (square)
        constexpr DataType Mat2_Float32 = makeDataTypeMatrix(DataSize::_4, DataKind::Float, 2, 2);
        constexpr DataType Mat3_Float32 = makeDataTypeMatrix(DataSize::_4, DataKind::Float, 3, 3);
        constexpr DataType Mat4_Float32 = makeDataTypeMatrix(DataSize::_4, DataKind::Float, 4, 4);
        constexpr DataType Mat2_Float64 = makeDataTypeMatrix(DataSize::_8, DataKind::Float, 2, 2);
        constexpr DataType Mat3_Float64 = makeDataTypeMatrix(DataSize::_8, DataKind::Float, 3, 3);
        constexpr DataType Mat4_Float64 = makeDataTypeMatrix(DataSize::_8, DataKind::Float, 4, 4);

        // matrices (non-square)
        constexpr DataType Mat3x4_Float32 = makeDataTypeMatrix(DataSize::_4, DataKind::Float, 3, 4);
        constexpr DataType Mat4x3_Float32 = makeDataTypeMatrix(DataSize::_4, DataKind::Float, 4, 3);
    }

    enum class DataType : std::int16_t { MetaFirst, Unknown = MetaFirst, UInt8, UInt16, UInt32, UInt64, Int8, Int16, Int32, Int64, Float16, Float32, Float64, MetaCount };

    extern int bytesize(const DataType dataType);
} // namespace xe

#endif
