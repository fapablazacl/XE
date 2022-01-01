
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
     * 
     * This struct encapsulates the scalar plane equation:
     * Ax + By + Cz = D
     */
    template<typename T>
    struct Plane {
        T a = T(0);
        T b = T(1);
        T c = T(0);
        T d = T(0);

        //! extracts the current normal vector
        Vector<T, 3> normal() const {
            return { a, b, c };
        }

        //! evaluate the result of the following equation:
        //! Ax + By + Cz - D
        T evaluate(const Vector<T, 3> &point) const {
            return dot(normal(), point) - d;
        }
        
        //! tests the point
        PlaneSide test(const Vector<T, 3> &point) const {
            const T det = evaluate(point);

            if (det < T(0)) {
                return PlaneSide::Back;
            } else if (det > T(0)) {
                return PlaneSide::Front;
            }

            return PlaneSide::Inside;
        }

        T* data() {
            return &a;
        }

        const T* data() const {
            return &a;
        }

    public:
        //! Create a Plane from its vectorial form (a Point and a Normal vector)
        static Plane<T> vectorial(const Vector<T, 3>& normal, const Vector<T, 3>& position) {
            return {
                normal.x, 
                normal.y, 
                normal.z,
                dot(position, normal)
            };
        }

        //! Initializes a plane from three points
        static Plane<T> triangle(const Vector<T, 3>& p1, const Vector<T, 3>& p2, const Vector<T, 3>& p3) {
            const auto normal = normalize(cross(p2 - p1, p3 - p1));
            const auto position = (p1 + p2 + p3) * (T(1) / T(3));
            
            return Plane<T>::vectorial( position, normal );
        }

        //! Creates a Plane aligned at the YZ-Plane
        static Plane<T> yz() {
            return {T(1), T(0), T(0), T(0)};
        }

        //! Creates a Plane aligned at the XZ-Plane
        static Plane<T> xz() {
            return {T(0), T(1), T(0), T(0)};
        }

        //! Creates a Plane aligned at the XY-Plane
        static Plane<T> xy() {
            return {T(0), T(0), T(1), T(0)};
        }

        //! Creates a Plane aligned at the YZ-Plane, at the specified position
        static Plane<T> yz(const Vector<T, 3>& position) {
            return Plane<T>::vectorial( position, {T(1), T(0), T(0)} );
        }

        //! Creates a Plane aligned at the XZ-Plane, at the specified position
        static Plane<T> xz(const Vector<T, 3>& position) {
            return Plane<T>::vectorial( position, {T(0), T(1), T(0)} );
        }

        //! Creates a Plane aligned at the XY-Plane, at the specified position
        static Plane<T> xy(const Vector<T, 3>& position) {
            return Plane<T>::vectorial( position, {T(0), T(0), T(1)} );
        }
    };
    
    //! Returns a inverted Plane, with the Normal part negated.
    template<typename T>
    Plane<T> invert(const Plane<T> &plane) {
        return {
            -plane.a,
            -plane.b,
            -plane.c,
            plane.d
        };
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
