/**
 * @file Common.hpp
 * @brief Common utility defitions and functions
 */

#ifndef __XE_MATH_COMMON_HPP__
#define __XE_MATH_COMMON_HPP__

#include <limits>
#include <cmath>
#include <cassert>
#include <string>
#include <sstream>

namespace XE {
    /**
     * @brief Perform a safe comparison between two floating point using a reference 'epsilon'
     */
    template<typename T>
    bool equals(const T a, const T b, const T epsilon = std::numeric_limits<T>::epsilon) {
        assert(epsilon >= 0.0f);

        if (a == b) {
            return true;
        } else {
            return std::abs(a - b) <= epsilon;
        }
    }

    template<typename T>
    std::string toString(const T value, const int precision) {
        std::ostringstream ss;
        
        ss.precision(precision);
        ss << std::fixed << value;

        return ss.str();
    }

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
