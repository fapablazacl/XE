
#ifndef __XE_STRUCTURE_HPP__
#define __XE_STRUCTURE_HPP__

#include <string>
#include <vector>
#include <type_traits>
#include <XE/DataType.hpp>

namespace XE {
    struct Attribute {
        DataType Type;
        int Dimension;
        int Count;
        std::string Name;
    };
    
    template<typename AttributeT>
    struct Structure {
        static_assert(std::is_base_of<Attribute, AttributeT>::value);

        std::vector<AttributeT> Attributes;
    };
}

#endif
