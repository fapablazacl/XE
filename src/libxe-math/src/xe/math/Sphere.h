/**
 * @file Sphere.h
 * @brief 3D sphere primitive and ray-sphere intersection.
 */

#pragma once

#include <cassert>
#include <cmath>
#include <limits>

#include "Ray.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Sphere in 3-space — centre and radius. Not part of glm.
     * The radius is asserted non-negative on construction; negative
     * radii are nonsense for any geometric purpose.
     */
    template <typename T> struct tsphere {
        tvec<T, 3> center{T(0), T(0), T(0)}; ///< Centre of the sphere in world space.
        T radius = static_cast<T>(1);        ///< Radius; non-negative.

        /**
         * @brief Default-construct to a unit sphere centred at the origin.
         */
        constexpr tsphere() noexcept = default;

        /**
         * @brief Construct a sphere centred at the origin with the given radius.
         * @param r Radius; must be non-negative (asserted).
         */
        constexpr explicit tsphere(T r) noexcept : radius(r) {
            assert(r >= static_cast<T>(0));
        }

        /**
         * @brief Construct a sphere with explicit centre and radius.
         * @param c Centre in world space.
         * @param r Radius; must be non-negative (asserted).
         */
        constexpr tsphere(const tvec<T, 3> &c, T r) noexcept : center(c), radius(r) {
            assert(r >= static_cast<T>(0));
        }
    };

    using sphere = tsphere<float>;   ///< Single-precision sphere.
    using dsphere = tsphere<double>; ///< Double-precision sphere.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TSphere = tsphere<T>; ///< @deprecated Use @ref tsphere.
    using Sphere = sphere;                            ///< @deprecated Use @ref sphere.
    using Sphered = dsphere;                          ///< @deprecated Use @ref dsphere.

    /**
     * @brief Ray-sphere intersection — returns the nearest hit parameter.
     * Solves the standard quadratic for the ray parameter @c t at which
     * @c r.pointAt(t) lies on the sphere's surface. When the discriminant
     * is exactly zero the ray is tangent to the sphere and the single
     * solution is returned; when the discriminant is positive the smaller
     * (nearer) root is returned; when there is no real solution
     * @c quiet_NaN is returned so the caller can branch on @c std::isnan
     * to detect a miss.
     * @param s Sphere to test against.
     * @param r Ray to test; @c r.direction is assumed unit-length.
     * @return Nearest hit parameter @c t, or @c quiet_NaN when no intersection exists.
     */
    template <typename T> [[nodiscard]] T test(const tsphere<T> &s, const tray<T> &r) {
        const tvec<T, 3> &r0 = r.origin;
        const tvec<T, 3> &d = r.direction;

        const tvec<T, 3> &c = s.center;
        const T radius2 = s.radius * s.radius;

        const tvec<T, 3> r0_sub_c = r0 - c;
        const T r0_c_2 = length2(r0_sub_c);

        const T B = T(2) * dot(d, r0_sub_c);
        const T C = r0_c_2 - radius2;

        const T disc = B * B - T(4) * C;

        if (disc == T(0)) {
            return -B / T(2);
        }
        if (disc > T(0)) {
            const T rootDisc = std::sqrt(disc);
            const T t1 = (-B - rootDisc) / T(2);
            const T t2 = (-B + rootDisc) / T(2);
            return t1 < t2 ? t1 : t2;
        }
        return std::numeric_limits<T>::quiet_NaN();
    }
} // namespace xe
