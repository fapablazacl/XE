/**
 * @file Ray.h
 * @brief 3D parametric ray — origin and direction pair.
 */

#pragma once

#include "Vector.h"

namespace xe {
    /**
     * @brief Ray in 3-space — origin plus direction.
     * Not part of glm. The field names follow the glm-ish convention
     * (@c origin and @c direction) instead of the older
     * @c position + @c direction pairing. Direction vectors are not
     * normalised on construction; the intersection routines that consume
     * a @c tray (for example @ref test for spheres and planes) assume the
     * direction is unit length, so the caller should normalize when
     * necessary.
     */
    template <typename T> struct tray {
        tvec<T, 3> origin{T(0), T(0), T(0)};    ///< Ray origin in world space.
        tvec<T, 3> direction{T(0), T(0), T(1)}; ///< Ray direction; expected to be normalised by intersection helpers.

        /**
         * @brief Default-construct to a ray at the origin pointing along @c +Z.
         */
        constexpr tray() noexcept = default;

        /**
         * @brief Construct from explicit origin and direction.
         * @param o Ray origin in world space.
         * @param d Ray direction; should be unit-length for intersection routines.
         */
        constexpr tray(const tvec<T, 3> &o, const tvec<T, 3> &d) noexcept : origin(o), direction(d) {
        }

        /**
         * @brief Compute the world-space point at parameter @p t along the ray.
         * Equivalent to @c origin @c + @c t @c * @c direction. Negative
         * values are valid; they yield points "behind" the origin along
         * the reversed direction.
         * @param t Parameter along the ray (typically distance when @c direction is unit-length).
         * @return The point @c origin @c + @c t @c * @c direction.
         */
        [[nodiscard]] constexpr tvec<T, 3> pointAt(T t) const noexcept {
            return origin + t * direction;
        }
    };

    using ray = tray<float>;   ///< Single-precision ray.
    using dray = tray<double>; ///< Double-precision ray.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TRay = tray<T>; ///< @deprecated Use @ref tray.
    using Ray = ray;                            ///< @deprecated Use @ref ray.
    using Rayd = dray;                          ///< @deprecated Use @ref dray.
} // namespace xe
