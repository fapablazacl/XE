
#ifndef __XE_GRAPHICS_UNIFORM_HPP__
#define __XE_GRAPHICS_UNIFORM_HPP__

#include <vector>
#include <string>
#include <XE/DataType.hpp>

namespace XE {
    /**
     * @brief A variable visible to all the stages in a ShaderProgram
     */
    struct Uniform {
        std::string Name;
        DataType Type;
        int Size;
        int Count;
    };

    /**
     * @brief A matrix visible to all the stages in a ShaderProgram
     */
    struct UniformMatrix {
        std::string Name;
        DataType Type;
        int Rows;
        int Columns;
        int Count;
    };
}

#endif
