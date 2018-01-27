
#ifndef __XE_DATATYPE_HPP__
#define __XE_DATATYPE_HPP__

#include <cstdint>

namespace XE {
    enum class DataType : std::int16_t {
        Unknown,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        Int8,
        Int16,
        Int32,
        Int64,
        Float16,
        Float32,
        Float64
    };

    extern int ComputeByteSize(const DataType dataType);
}

#endif
