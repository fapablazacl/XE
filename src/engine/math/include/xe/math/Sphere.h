
#ifndef __XE_MATH_SPHERE_HPP__
#define __XE_MATH_SPHERE_HPP__

#include "Vector.h"

#include <cassert>
#include <limits>

namespace XE {
    /**
     * @brief Sphere in the 3-space
     */
    template <typename T> struct Sphere {
        TVector<T, 3> center;
        T radius = static_cast<T>(1);

        Sphere() : center({static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)}), radius(static_cast<T>(1)) {}

        /**
         * @brief Initializes a sphere in the origin with the specified radius
         */
        explicit Sphere(T radius) : radius(radius) { assert(radius >= static_cast<T>(0)); }

        /**
         * @brief Initializes a Sphere from the given center and radius
         */
        Sphere(const TVector<T, 3> &center, T radius) : center(center), radius(radius) { assert(radius >= static_cast<T>(0)); }
    };

    extern template struct Sphere<float>;
    extern template struct Sphere<double>;

    template <typename T> struct Ray;

    /**
     * @brief Test for collisions between a Sphere and a Ray
     */
    template <typename T> T test(const Sphere<T> &sphere, const Ray<T> &ray) {
        const TVector<T, 3> &r0 = ray.position;
        const TVector<T, 3> &d = ray.direction;

        const TVector<T, 3> &c = sphere.center;
        const T r = sphere.radius;
        const T r_2 = r * r;

        const TVector<T, 3> r0_sub_c = r0 - c;
        const T r0_c_2 = norm2(r0_sub_c);

        const T B = T(2) * dot(d, r0_sub_c);
        const T C = r0_c_2 - r_2;

        const T disc = B * B - T(4) * C;

        if (disc == T(0)) {
            return -B / T(2);
        } else if (disc > T(0)) {
            T rootDisc = std::sqrt(disc);

            T t1 = (-B - rootDisc) / T(2);
            T t2 = (-B + rootDisc) / T(2);

            return std::min(t1, t2);
        } else {
            return std::numeric_limits<T>::quiet_NaN();
        }
    }
} // namespace XE

#endif
