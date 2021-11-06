
#ifndef __XE_GRAPHICS_UNIFORM_HPP__
#define __XE_GRAPHICS_UNIFORM_HPP__

#include <vector>
#include <string>
#include <XE/DataType.h>

namespace XE {
    enum class UniformDimension : int {
        D1 = 1,
        D2,
        D3,
        D4
    };

    /**
     * @brief A variable visible to all the stages in a ShaderProgram
     */
    struct Uniform {
        std::string Name;
        DataType Type;
        UniformDimension dimension;
        int Count;
    };

    enum class UniformMatrixShape : int {
        R2C2,
        R2C3,
        R2C4,
        R3C2,
        R3C3,
        R3C4,
        R4C2,
        R4C3,
        R4C4
    };

    /**
     * @brief A matrix visible to all the stages in a ShaderProgram
     */
    struct UniformMatrix {
        std::string Name;
        DataType Type;
        UniformMatrixShape shape;
        int Count;
    };
}

#endif
