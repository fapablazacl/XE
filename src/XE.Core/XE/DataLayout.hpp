
#ifndef __XE_DATALAYOUT_HPP__
#define __XE_DATALAYOUT_HPP__

#include <string>
#include <vector>
#include <XE/DataType.hpp>

namespace XE {
    struct DataAttribute {
        DataType Type;
        std::int16_t Dimension;
        std::int16_t Count;
        char Name[16];
    };

    /**
     * @brief Description for packed data structures
     */
    template<typename AttributeT>
    struct DataLayout {
        static_assert(std::is_base_of<DataAttribute, AttributeT>::value, "Template parameter 'AttributeT' must be a derived type from Pack<T, N>::AttributeBase");

        std::vector<AttributeT> Attributes;
    };
}

#endif
