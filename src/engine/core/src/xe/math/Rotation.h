
#ifndef __XE_MATH_ROTATION_HPP__
#define __XE_MATH_ROTATION_HPP__

#include "Vector.h"

namespace XE {
    template<typename T>
    struct Rotation {
        T angle;
        Vector<T, 3> axis;
    };

}

#endif
