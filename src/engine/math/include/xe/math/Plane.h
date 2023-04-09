
#ifndef __XE_MATH_PLANE_HPP__
#define __XE_MATH_PLANE_HPP__

#include "Vector.h"

#include <ostream>

namespace XE {
    enum class PlaneSide { Front, Back, Inside };

    /**
     * @brief Plane on 3-space.
     *
     * This struct encapsulates the scalar plane equation:
     * Ax + By + Cz = D
     */
    template <typename T> struct Plane {
        T a = T(0);
        T b = T(1);
        T c = T(0);
        T d = T(0);

        explicit Plane() {}

        explicit Plane(const T a, const T b, const T c, const T d) : a(a), b(b), c(c), d(d) {}

        explicit Plane(const TVector3<T> &n, const T d) : a(n.X), b(n.Y), c(n.Z), d(d) {}

        /**
         * @brief Returns the current normal vector
         *
         * @return TVector3<T>
         */
        TVector3<T> normal() const { return {a, b, c}; }

        /**
         * @brief Evaluate the point coordinates in the scalar Plane equation.
         *
         * Evaluate the result of the following equation:
         * Ax + By + Cz - D
         *
         * where A, B, C and D are the scalar plane equation coefficients, represented in the Plane struct,
         * and the coordinates (x, y, z) are taken from the input point to evaluate.
         *
         * The result can be used later to determine how the point is related to the plane.
         *
         * @param point The point to evaluate in 3-space.
         * @return T
         */
        T evaluate(const TVector3<T> &point) const { return dot(normal(), point) - d; }

        bool operator==(const Plane<T> &rhs) const { return (a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d); }

        bool operator!=(const Plane<T> &rhs) const { return !(*this == rhs); }

        /**
         * @brief Determines whenever two planes intersects or not.
         *
         * Two planes intersects only if their normals are not parallel.
         * This method computes the dot product between the two normals, and then checks the resulting value.
         *
         * @note This method assumes that both normals ("this->normal" and "other.normal") are normalized for performance reasons.
         * @param other Another plane to test intersection with,
         * @return true if the dot product result is not one.
         * @return false if the dot product result is one.
         */
        bool intersect(const Plane<T> &other) const {
            const T product = dot(normal(), other.normal());

            return product != static_cast<T>(1);
        }

        /**
         * @brief tests the point against the Plane.
         *
         * @param point
         * @return PlaneSide
         */
        PlaneSide test(const TVector3<T> &point) const {
            const T det = evaluate(point);

            if (det < T(0)) {
                return PlaneSide::Back;
            } else if (det > T(0)) {
                return PlaneSide::Front;
            }

            return PlaneSide::Inside;
        }

        /**
         * @brief Returns a pointer to the raw data in the Plane.
         *
         * @return T*
         */
        T *data() { return &a; }

        /**
         * @brief Returns a pointer to the raw data in the Plane.
         *
         * @return const T*
         */
        const T *data() const { return &a; }

    public:
        /**
         * @brief Create a Plane from its vectorial form (a Point and a Normal vector)
         *
         * @param normal
         * @param position
         * @return Plane<T>
         */
        static Plane<T> vectorial(const TVector3<T> &normal, const TVector3<T> &position) { return Plane<T>{normalize(normal), dot(position, normal)}; }

        /**
         * @brief Initializes a plane from three points
         *
         * @param p1
         * @param p2
         * @param p3
         * @return Plane<T>
         */
        static Plane<T> triangle(const TVector3<T> &p1, const TVector3<T> &p2, const TVector3<T> &p3) {
            const auto normal = normalize(cross(p2 - p1, p3 - p1));
            const auto position = (p1 + p2 + p3) * (T(1) / T(3));

            return Plane<T>::vectorial(position, normal);
        }

        /**
         * @brief Creates a Plane aligned at the YZ-Plane
         *
         * @return Plane<T>
         */
        static Plane<T> yz() { return Plane<T>{T(1), T(0), T(0), T(0)}; }

        /**
         * @brief Creates a Plane aligned at the XZ-Plane
         *
         * @return Plane<T>
         */
        static Plane<T> xz() { return Plane<T>{T(0), T(1), T(0), T(0)}; }

        /**
         * @brief Creates a Plane aligned at the XY-Plane
         *
         * @return Plane<T>
         */
        static Plane<T> xy() { return Plane<T>{T(0), T(0), T(1), T(0)}; }

        /**
         * @brief Creates a Plane aligned at the YZ-Plane, at the specified position
         *
         * @param position
         * @return Plane<T>
         */
        static Plane<T> yz(const TVector3<T> &position) { return Plane<T>{T(1), T(0), T(0), position.X}; }

        /**
         * @brief Creates a Plane aligned at the XZ-Plane, at the specified position
         *
         * @param position
         * @return Plane<T>
         */
        static Plane<T> xz(const TVector3<T> &position) { return Plane<T>{T(0), T(1), T(0), position.Y}; }

        /**
         * @brief Creates a Plane aligned at the XY-Plane, at the specified position
         *
         * @param position
         * @return Plane<T>
         */
        static Plane<T> xy(const TVector3<T> &position) { return Plane<T>{T(0), T(0), T(1), position.Z}; }
    };

    /**
     * @brief Returns a inverted Plane, with the Normal part negated.
     *
     * @tparam T
     * @param plane
     * @return Plane<T>
     */
    template <typename T> Plane<T> invert(const Plane<T> &plane) { return {-plane.a, -plane.b, -plane.c, plane.d}; }

    template <typename T> struct Ray;

    /**
     * @brief Test for collisions between a Plane and a Ray
     * dot(P, N) = d    (1)
     * P = R + t*D      (2)
     * t = (d - R * N) / (D * N)
     *
     * @tparam T The data type
     * @param plane
     * @param ray
     * @todo Missing the validation when there is no intersection between the Plane and the Ray
     * @return T The computed t scalar factor
     */
    template <typename T> T test(const Plane<T> &plane, const Ray<T> &ray) {
        const TVector3<T> n = plane.normal();
        const T num = plane.d - dot(ray.point, n);
        const T dem = dot(ray.direction, n);

        return num / dem;
    }

    //! Plane specialization with the float data type
    using Planef = Plane<float>;

    //! Plane specialization with the double data type
    using Planed = Plane<double>;

    /**
     * @brief Serializes a Plane using the supplied ostream
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const Plane<T> &plane) {
        os << "XE::Plane<" << typeid(T).name() << ">{ ";

        os << plane.a << ", ";
        os << plane.b << ", ";
        os << plane.c << ", ";
        os << plane.d << " }";

        return os;
    }

    extern template struct Plane<float>;
    extern template struct Plane<double>;
} // namespace XE

#endif
