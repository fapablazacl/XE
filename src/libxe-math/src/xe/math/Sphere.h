#pragma once

#include <cassert>
#include <cmath>
#include <limits>

#include "Ray.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Sphere in 3-space. Not part of glm.
     */
    template <typename T> struct tsphere {
        tvec<T, 3> center{T(0), T(0), T(0)};
        T radius = static_cast<T>(1);

        constexpr tsphere() noexcept = default;

        constexpr explicit tsphere(T r) noexcept : radius(r) {
            assert(r >= static_cast<T>(0));
        }

        constexpr tsphere(const tvec<T, 3> &c, T r) noexcept : center(c), radius(r) {
            assert(r >= static_cast<T>(0));
        }
    };

    using sphere = tsphere<float>;
    using dsphere = tsphere<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TSphere = tsphere<T>;
    using Sphere = sphere;
    using Sphered = dsphere;

    /**
     * @brief Ray-sphere intersection. Returns the nearest hit parameter, or NaN
     * when there is no intersection.
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
