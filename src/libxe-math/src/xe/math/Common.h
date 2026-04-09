/**
 * @file Common.h
 * @brief Common math constants and scalar helpers.
 *
 * Names and semantics match GLM where possible, so xe::pi, xe::radians,
 * xe::degrees, xe::lerp, xe::equals, and xe::epsilon behave as their glm::
 * counterparts.
 */

#pragma once

#include <cmath>
#include <cstdlib>

namespace xe {
    //! Default infinitesimal used by @ref equals when no explicit epsilon is supplied.
    template <typename T> inline constexpr T epsilon = static_cast<T>(1e-6);

    //! Pi constant.
    template <typename T> inline constexpr T pi = static_cast<T>(3.141592653589793238);

    //! Absolute-difference comparison against the default @ref epsilon.
    template <typename T> [[nodiscard]] constexpr bool equals(const T a, const T b) noexcept {
        return (a > b ? a - b : b - a) <= epsilon<T>;
    }

    //! Absolute-difference comparison against a caller-supplied epsilon.
    template <typename T> [[nodiscard]] constexpr bool equals(const T a, const T b, const T eps) noexcept {
        return (a > b ? a - b : b - a) <= eps;
    }

    //! Convert degrees to radians.
    template <typename T> [[nodiscard]] constexpr T radians(const T degrees) noexcept {
        return degrees * pi<T> / static_cast<T>(180);
    }

    //! Convert radians to degrees.
    template <typename T> [[nodiscard]] constexpr T degrees(const T radians) noexcept {
        return radians * static_cast<T>(180) / pi<T>;
    }

    //! Scalar linear interpolation: result = v1 + (v2 - v1) * s.
    template <typename T, typename S> [[nodiscard]] constexpr T lerp(const T v1, const T v2, const S s) noexcept {
        return v1 + (v2 - v1) * s;
    }
} // namespace xe
