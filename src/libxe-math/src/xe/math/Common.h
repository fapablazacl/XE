/**
 * @file Common.h
 * @brief Common math constants and scalar helpers shared across xe::math.
 *
 * Names and semantics match GLM where possible, so @ref xe::pi,
 * @ref xe::radians, @ref xe::degrees, @ref xe::lerp, @ref xe::equals, and
 * @ref xe::epsilon behave as drop-in replacements for their @c glm::
 * counterparts. Everything in this header is @c constexpr and free of any
 * runtime allocation, so it is safe to use inside other constant
 * expressions and template metaprogramming.
 */

#pragma once

#include <cmath>
#include <cstdlib>

namespace xe {
    /**
     * @brief Default infinitesimal used by @ref equals when no explicit tolerance is supplied.
     * Set to @c 1e-6 cast to @p T. The value is intentionally tight enough to
     * catch unintended drift between two computed quantities while still
     * tolerating the rounding noise of single-precision floating-point math.
     * Specialise the template if a different default is required for a
     * specific scalar type.
     */
    template <typename T> inline constexpr T epsilon = static_cast<T>(1e-6);

    /**
     * @brief Mathematical constant π.
     * Provided as an inline @c constexpr template variable so it can be used
     * inside other constant expressions without paying the cost of @c std::acos
     * at runtime. Use this in preference to hard-coding the literal so the
     * precision matches the active scalar type.
     */
    template <typename T> inline constexpr T pi = static_cast<T>(3.141592653589793238);

    /**
     * @brief Absolute-difference comparison against the default @ref epsilon.
     * Useful when chaining floating-point arithmetic where exact equality
     * cannot be relied on. The comparison is symmetric and uses an absolute
     * (not relative) tolerance, so it is most reliable for values close to
     * zero or of similar magnitude.
     * @param a First value to compare.
     * @param b Second value to compare.
     * @return @c true when @c |a - b| is less than or equal to @ref epsilon "epsilon<T>".
     */
    template <typename T> [[nodiscard]] constexpr bool equals(const T a, const T b) noexcept {
        return (a > b ? a - b : b - a) <= epsilon<T>;
    }

    /**
     * @brief Absolute-difference comparison against a caller-supplied tolerance.
     * Identical to the two-argument overload but lets the caller widen or
     * tighten the tolerance for a specific call site (for example, when
     * comparing accumulated values whose error budget is known).
     * @param a First value to compare.
     * @param b Second value to compare.
     * @param eps Maximum permitted absolute difference, in the same units as @p a and @p b.
     * @return @c true when @c |a - b| is less than or equal to @p eps.
     */
    template <typename T> [[nodiscard]] constexpr bool equals(const T a, const T b, const T eps) noexcept {
        return (a > b ? a - b : b - a) <= eps;
    }

    /**
     * @brief Convert an angle expressed in degrees to radians.
     * Provided so caller code can stay in the more familiar degree units while
     * the rest of xe::math (matrix builders, quaternions, trigonometric helpers)
     * continues to operate in radians, the canonical unit.
     * @param degrees Angle in degrees.
     * @return The same angle expressed in radians.
     */
    template <typename T> [[nodiscard]] constexpr T radians(const T degrees) noexcept {
        return degrees * pi<T> / static_cast<T>(180);
    }

    /**
     * @brief Convert an angle expressed in radians to degrees.
     * The inverse of @ref radians; intended for presentation, logging, and
     * test assertions where degrees are easier to read than radians.
     * @param radians Angle in radians.
     * @return The same angle expressed in degrees.
     */
    template <typename T> [[nodiscard]] constexpr T degrees(const T radians) noexcept {
        return radians * static_cast<T>(180) / pi<T>;
    }

    /**
     * @brief Scalar linear interpolation between two values.
     * Computes @c v1+(v2-v1)*s, the canonical lerp formulation. The
     * interpolation is unclamped — passing @p s outside @c [0,1] extrapolates
     * past the endpoints, which is intentional for animation and easing
     * pipelines that may overshoot.
     * @param v1 Value returned when @p s is 0.
     * @param v2 Value returned when @p s is 1.
     * @param s Interpolation parameter; usually in @c [0,1] but not enforced.
     * @return The interpolated (or extrapolated) value.
     */
    template <typename T, typename S> [[nodiscard]] constexpr T lerp(const T v1, const T v2, const S s) noexcept {
        return v1 + (v2 - v1) * s;
    }
} // namespace xe
