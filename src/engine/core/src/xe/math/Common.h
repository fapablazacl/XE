/**
 * @file Common.hpp
 * @brief Common utility defitions and functions
 */

#ifndef __XE_MATH_COMMON_HPP__
#define __XE_MATH_COMMON_HPP__

#include <cassert>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>

namespace XE {
    //! Perform a safe comparison between two floating point using a reference 'epsilon'
    template <typename T> bool equals(const T a, const T b, const T epsilon = std::numeric_limits<T>::epsilon()) { return std::abs(a - b) <= epsilon; }

    //! Variable Template that contains the PI value
    template <typename T> inline const T pi = static_cast<T>(3.141592653589793238);

    //! Converts Degrees to Radians.
    template <typename T> constexpr T radians(const T degrees) { return degrees * pi<T> / static_cast<T>(180); }

    //! Converts Radians to Degrees
    template <typename T> constexpr T degrees(const T radians) { return radians * static_cast<T>(180) / pi<T>; }

    //! Performs a Linear Interpolation between two values.
    template <typename T, typename S> T lerp(const T v1, const T v2, const S s) { return v1 + (v2 - v1) * s; }
} // namespace XE

#endif
