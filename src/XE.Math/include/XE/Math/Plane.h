
#ifndef __XE_MATH_PLANE_HPP__
#define __XE_MATH_PLANE_HPP__

#include "Vector.h"

namespace XE {
    enum class PlaneSide {
        Front,
        Back,
        Inside
    };

    /**
     * @brief Plane on 3-space.
     */
    template<typename T>
    struct Plane {
        Vector<T, 3> position;
        Vector<T, 3> normal;

    public:
        PlaneSide test(const Vector<T, 3> &point) const {
            return PlaneSide::Inside;
        }

    public:
        //! Initializes a plane from three points
        static Plane<T> triangle(const Vector<T, 3>& p1, const Vector<T, 3>& p2, const Vector<T, 3>& p3) {
            const auto position = (p1 + p2 + p3) * (T(1) / T(3));
            const auto normal = normalize(cross(p2 - p1, p3 - p1));

            return { position, normal };
        }

        //! Creates a Plane aligned at the XY-Plane
        static Plane<T> xy() {
            return {
                {T(0), T(0), T(0)},
                {T(0), T(0), T(1)}
            };
        }

        //! Creates a Plane aligned at the YZ-Plane
        static Plane<T> yz() {
            return {
                {T(0), T(0), T(0)},
                {T(1), T(0), T(0)}
            };
        }

        //! Creates a Plane aligned at the XZ-Plane
        static Plane<T> xz() {
            return {
                {T(0), T(0), T(0)},
                {T(0), T(1), T(0)}
            };
        }
    };

    //! Returns a inverted Plane, with the Normal negated.
    template<typename T>
    Plane<T> invert(const Plane<T> &plane) {
        return { plane.position, -plane.normal };
    }

    template<typename T>
    struct Ray;

    /**
     * @brief Test for collisions between a Plane and a Ray
     */
    template<typename T>
    T test(const Plane<T> &plane, const Ray<T>& ray) {
        auto p = plane.position;
        auto n = plane.normal;
        
        auto r = ray.point;
        auto d = ray.direction;
        
        return dot(n, p - r) / dot(n, d);
    }

    using Planef = Plane<float>;
    using Planed = Plane<double>;
}

#endif
