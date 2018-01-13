#ifndef __XE_DATATYPE_HPP__
#define __XE_DATATYPE_HPP__

#include <cstdint>

namespace XE {
    enum class DataType {
        Unknown,
        UInt8,      UInt16, UInt32,
        Int8,       Int16,  Int32,
        Float16,    Float32, Float64
    };
    
    inline bool isFloat(DataType type) {
        return type==DataType::Float16 || type==DataType::Float32;
    }
    
    inline bool isInt(DataType type) {
        return type==DataType::Int8 || type==DataType::Int16 || type==DataType::Int32;
    }
    
    inline bool isUInt(DataType type) {
        return type==DataType::UInt8 || type==DataType::UInt16 || type==DataType::UInt32;
    }
    
    template<typename Type>
    constexpr DataType getDataType() {return DataType::Unknown;}
    
    template<>
    constexpr DataType getDataType<std::int32_t>() {return DataType::Int32;}
    
    template<>
    constexpr DataType getDataType<float>() {return DataType::Float32;}
    
    inline std::size_t getSize(DataType dataType)  {
        switch (dataType) {
            case DataType::UInt8:
            case DataType::Int8:
                return 1;
                
            case DataType::UInt16:
            case DataType::Int16:
            case DataType::Float16:
                return 2;
                
            case DataType::UInt32:
            case DataType::Int32:
            case DataType::Float32:
                return 4;
                
            default:
                return 0;
        }
    }
}

#endif
