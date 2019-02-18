
#ifndef __XE_MATH_UTIL_HPP__
#define __XE_MATH_UTIL_HPP__

#include <cmath>

namespace XE {
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

    template<typename T, typename S>
    T lerp(const T v1, const T v2, const S s) {
        return v1 + (v2 - v1) * s;
    }
}

#endif
