
#ifndef __XE_GRAPHICS_ATTRIBUTE_HPP__
#define __XE_GRAPHICS_ATTRIBUTE_HPP__

#include <string>
#include <vector>
#include <XE/DataType.hpp>

namespace XE::Graphics {
    struct Attribute {
        int BufferIndex;
        DataType Type;
        int Dimension;
        std::string Name;
    };
    
    struct Structure {
        std::vector<Attribute> Attributes;
    };
}

#endif
