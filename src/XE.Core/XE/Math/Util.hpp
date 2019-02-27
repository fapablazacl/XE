
#ifndef __XE_MATH_UTIL_HPP__
#define __XE_MATH_UTIL_HPP__

#include <cmath>

namespace XE {
    template<typename T>
    inline const T pi = static_cast<T>(3.141592653589793238);

    template<typename T>
    inline T radians(const T degrees) {
        return degrees * pi<T> / static_cast<T>(180);
    }

    template<typename T>
    inline T degrees(const T radians) {
        return radians * static_cast<T>(180) / pi<T>;
    }

    template<typename T, typename S>
    T lerp(const T v1, const T v2, const S s) {
        return v1 + (v2 - v1) * s;
    }
}

#endif
