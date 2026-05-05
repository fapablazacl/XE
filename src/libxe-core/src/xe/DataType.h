
#ifndef __XE_DATATYPE_HPP__
#define __XE_DATATYPE_HPP__

#include <cstdint>

namespace xe {
    namespace internal {
        struct DataTypeDesc {
            uint16_t bytes:2;   // 0=1-byte, 1=2-byte, 2=4-byte, 3=8-byte per element
            uint16_t type:2;    // 0=uint, 1=int, 2=float
            uint16_t cols:3;    // 1=scalar, 2-4=vector/matrix columns
            uint16_t rows:3;    // 0=scalar/vector, 2-4=matrix rows
        };

        //! bitfield-encoded datatype descriptor
        using DataType = uint16_t;

        static_assert(sizeof(DataTypeDesc) == sizeof(DataType));

        enum class DataTypeBytes : uint8_t {
            _1BYTE,
            _2BYTE,
            _4BYTE,
            _8BYTE,
        };

        enum class DataTypeType : uint8_t {
            UINT,
            INT,
            FLOAT,
        };

        constexpr DataType makeDataTypeScalar(DataTypeBytes bytes, DataTypeType type) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & 0x3u) |
                ((static_cast<uint16_t>(type) & 0x3u) << 2u) |
                (1u << 4u)
            );
        }

        constexpr DataType makeDataTypeVector(DataTypeBytes bytes, DataTypeType type, uint8_t dim) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & 0x3u) |
                ((static_cast<uint16_t>(type) & 0x3u) << 2u) |
                ((static_cast<uint16_t>(dim) & 0x7u) << 4u)
            );
        }

        constexpr DataType makeDataTypeMatrix(DataTypeBytes bytes, DataTypeType type, uint8_t cols, uint8_t rows) {
            return static_cast<DataType>(
                (static_cast<uint16_t>(bytes) & 0x3u) |
                ((static_cast<uint16_t>(type) & 0x3u) << 2u) |
                ((static_cast<uint16_t>(cols) & 0x7u) << 4u) |
                ((static_cast<uint16_t>(rows) & 0x7u) << 7u)
            );
        }

        // scalars
        constexpr DataType Scalar_UInt8   = makeDataTypeScalar(DataTypeBytes::_1BYTE, DataTypeType::UINT);
        constexpr DataType Scalar_UInt16  = makeDataTypeScalar(DataTypeBytes::_2BYTE, DataTypeType::UINT);
        constexpr DataType Scalar_UInt32  = makeDataTypeScalar(DataTypeBytes::_4BYTE, DataTypeType::UINT);
        constexpr DataType Scalar_UInt64  = makeDataTypeScalar(DataTypeBytes::_8BYTE, DataTypeType::UINT);
        constexpr DataType Scalar_Int8    = makeDataTypeScalar(DataTypeBytes::_1BYTE, DataTypeType::INT);
        constexpr DataType Scalar_Int16   = makeDataTypeScalar(DataTypeBytes::_2BYTE, DataTypeType::INT);
        constexpr DataType Scalar_Int32   = makeDataTypeScalar(DataTypeBytes::_4BYTE, DataTypeType::INT);
        constexpr DataType Scalar_Int64   = makeDataTypeScalar(DataTypeBytes::_8BYTE, DataTypeType::INT);
        constexpr DataType Scalar_Float16 = makeDataTypeScalar(DataTypeBytes::_2BYTE, DataTypeType::FLOAT);
        constexpr DataType Scalar_Float32 = makeDataTypeScalar(DataTypeBytes::_4BYTE, DataTypeType::FLOAT);
        constexpr DataType Scalar_Float64 = makeDataTypeScalar(DataTypeBytes::_8BYTE, DataTypeType::FLOAT);

        // vectors
        constexpr DataType Vec2_UInt8   = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::UINT,  2);
        constexpr DataType Vec3_UInt8   = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::UINT,  3);
        constexpr DataType Vec4_UInt8   = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::UINT,  4);
        constexpr DataType Vec2_UInt16  = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::UINT,  2);
        constexpr DataType Vec3_UInt16  = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::UINT,  3);
        constexpr DataType Vec4_UInt16  = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::UINT,  4);
        constexpr DataType Vec2_UInt32  = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::UINT,  2);
        constexpr DataType Vec3_UInt32  = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::UINT,  3);
        constexpr DataType Vec4_UInt32  = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::UINT,  4);
        constexpr DataType Vec2_Int8    = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::INT,   2);
        constexpr DataType Vec3_Int8    = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::INT,   3);
        constexpr DataType Vec4_Int8    = makeDataTypeVector(DataTypeBytes::_1BYTE, DataTypeType::INT,   4);
        constexpr DataType Vec2_Int16   = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::INT,   2);
        constexpr DataType Vec3_Int16   = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::INT,   3);
        constexpr DataType Vec4_Int16   = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::INT,   4);
        constexpr DataType Vec2_Int32   = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::INT,   2);
        constexpr DataType Vec3_Int32   = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::INT,   3);
        constexpr DataType Vec4_Int32   = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::INT,   4);
        constexpr DataType Vec2_Float16 = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::FLOAT, 2);
        constexpr DataType Vec3_Float16 = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::FLOAT, 3);
        constexpr DataType Vec4_Float16 = makeDataTypeVector(DataTypeBytes::_2BYTE, DataTypeType::FLOAT, 4);
        constexpr DataType Vec2_Float32 = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 2);
        constexpr DataType Vec3_Float32 = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 3);
        constexpr DataType Vec4_Float32 = makeDataTypeVector(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 4);
        constexpr DataType Vec2_Float64 = makeDataTypeVector(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 2);
        constexpr DataType Vec3_Float64 = makeDataTypeVector(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 3);
        constexpr DataType Vec4_Float64 = makeDataTypeVector(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 4);

        // matrices (square)
        constexpr DataType Mat2_Float32 = makeDataTypeMatrix(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 2, 2);
        constexpr DataType Mat3_Float32 = makeDataTypeMatrix(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 3, 3);
        constexpr DataType Mat4_Float32 = makeDataTypeMatrix(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 4, 4);
        constexpr DataType Mat2_Float64 = makeDataTypeMatrix(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 2, 2);
        constexpr DataType Mat3_Float64 = makeDataTypeMatrix(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 3, 3);
        constexpr DataType Mat4_Float64 = makeDataTypeMatrix(DataTypeBytes::_8BYTE, DataTypeType::FLOAT, 4, 4);

        // matrices (non-square)
        constexpr DataType Mat3x4_Float32 = makeDataTypeMatrix(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 3, 4);
        constexpr DataType Mat4x3_Float32 = makeDataTypeMatrix(DataTypeBytes::_4BYTE, DataTypeType::FLOAT, 4, 3);
    }

    enum class DataType : std::int16_t { MetaFirst, Unknown = MetaFirst, UInt8, UInt16, UInt32, UInt64, Int8, Int16, Int32, Int64, Float16, Float32, Float64, MetaCount };

    extern int bytesize(const DataType dataType);
} // namespace xe

#endif
