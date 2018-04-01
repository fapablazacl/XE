
#ifndef __XE_MATH_UTIL_HPP__
#define __XE_MATH_UTIL_HPP__

#include <cmath>

namespace XE::Math {
    template<typename T>
    inline const T Pi = static_cast<T>(3.141592653589793238);

    template<typename T>
    inline T Radians(const T degrees) {
        return degrees * Pi<T> / static_cast<T>(180);
    }

    template<typename T>
    inline T Degrees(const T radians) {
        return radians * static_cast<T>(180) / Pi<T>;
    }
}

#endif
