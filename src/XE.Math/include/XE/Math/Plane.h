
#ifndef __XE_MATH_PLANE_HPP__
#define __XE_MATH_PLANE_HPP__

#include "Vector.h"

namespace XE {
    /**
     * @brief Plane on 3-space.
     */
    template<typename T>
    struct Plane {
        Vector<T, 3> position;
        Vector<T, 3> Normal;

        /**
         * @brief Initialize a plane from three points. These points must be coplanar.
         */
        static Plane<T> FromTriangle(const Vector<T, 3>& p1, const Vector<T, 3>& p2, const Vector<T, 3>& p3) {
            return {
                (p1 + p2 + p3) / T(3),
                normalize(cross(p2 - p1, p3 - p1))
            };
        }

        static Plane<T> MakeXY() {
            return {
                {T(0), T(0), T(0)},
                {T(0), T(0), T(1)}
            };
        }

        static Plane<T> MakeYZ() {
            return {
                {T(0), T(0), T(0)},
                {T(1), T(0), T(0)}
            };
        }

        static Plane<T> MakeXZ() {
            return {
                {T(0), T(0), T(0)},
                {T(0), T(1), T(0)}
            };
        }
    };

    template<typename T>
    struct Ray;

    /**
     * @brief Test for collisions between a Plane and a Ray
     */
    template<typename T>
    T test(const Plane<T> &plane, const Ray<T>& ray) {
        auto p = plane.Point;
        auto n = plane.Normal;
        
        auto r = ray.Point;
        auto d = ray.direction;
        
        return dot(n, p - r) / dot(n, d);
    }

    /**
     * @brief Inverts the side of the plane
     */
    template<typename T>
    Plane<T> Invert(const Plane<T> &other) {
        return { other.position, -other.Normal };
    }

    typedef Plane<float> Plane_f;
    typedef Plane<double> Plane_d;
}

#endif
