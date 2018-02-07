
#ifndef __XE_DATALAYOUT_HPP__
#define __XE_DATALAYOUT_HPP__

#include <vector>

namespace XE {
    struct DataAttribute;

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
