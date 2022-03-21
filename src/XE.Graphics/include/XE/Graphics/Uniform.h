
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
        std::string name;
        DataType type;
        UniformDimension dimension;
        int count = 1;
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
        std::string name;
        DataType type;
        UniformMatrixShape shape;
        int count = 1;
    };

    
    inline int countElements(const UniformDimension dim) {
        switch (dim) {
            case UniformDimension::D1: return 1;
            case UniformDimension::D2: return 2;
            case UniformDimension::D3: return 3;
            case UniformDimension::D4: return 4;
        }

        return -1;
    }


    inline int countElements(const UniformMatrixShape shape) {
        switch (shape) {
            case UniformMatrixShape::R2C2: return 2*2;
            case UniformMatrixShape::R2C3: return 2*3;
            case UniformMatrixShape::R2C4: return 2*4;
            case UniformMatrixShape::R3C2: return 3*2;
            case UniformMatrixShape::R3C3: return 3*3;
            case UniformMatrixShape::R3C4: return 3*4;
            case UniformMatrixShape::R4C2: return 4*2;
            case UniformMatrixShape::R4C3: return 4*3;
            case UniformMatrixShape::R4C4: return 4*4;
        }

        return -1;
    }
}

#endif
