
#ifndef __XE_DATATYPE_HPP__
#define XE_DATATYPE_HPP_

#include <cstdint>

namespace XE {
    enum class DataType : std::int16_t { MetaFirst = 0, Unknown = MetaFirst, UInt8 = 1, UInt16 = 2, UInt32 = 3, UInt64 = 4, Int8 = 5, Int16 = 6, Int32 = 7, Int64 = 8, Float16 = 9, Float32 = 10, Float64 = 11, MetaCount = 12 };

    extern int bytesize(DataType dataType);
} // namespace XE

#endif
