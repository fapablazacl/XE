
#ifndef __XE_MATH_BOUNDARY_HPP__
#define __XE_MATH_BOUNDARY_HPP__

#include <array>
#include <cassert>
#include <limits>

#include "Range.h"
#include "Vector.h"

namespace XE {
    /**
     * @brief N-dimensional boundary class
     *
     * Used as a basis for implementing boundary logic in specific dimensions, like Rect (for 2-space), and Box (for 3-space).
     *
     * @note Consider replacing its implementation based on Vectors onto another more abstract entity
     *
     * @tparam T
     * @tparam N
     */
    template <typename T, int N> class Boundary {
    public:
        static_assert(N == 2 || N == 3, "N template parameter must one of (2, 3)");

    public:
        template <int Base, int Exp> struct Power {
            enum { Value = Base * Power<Base, Exp - 1>::Value };
        };

        template <int Base> struct Power<Base, 0> {
            enum { Value = 1 };
        };

    public:
        enum { SideCount = 2 * N };

        enum { PointCount = Power<2, N>::Value };

        enum { MinEdge = 0, MaxEdge = PointCount - 1 };

    public:
        Boundary() {}

        explicit Boundary(const TVector<T, N> &value) : minEdge(value), maxEdge(value) {}

        Boundary(const TVector<T, N> &value1, const TVector<T, N> &value2) : Boundary(value1) { expand(value2); }

        void expand(const TVector<T, N> &value) {
            minEdge = minimize(minEdge, value);
            maxEdge = maximize(maxEdge, value);
        }

        void expand(const Boundary<T, N> &other) {
            expand(other.minEdge);
            expand(other.maxEdge);
        }

        TVector<T, N> getMinEdge() const { return minEdge; }

        TVector<T, N> getMaxEdge() const { return maxEdge; }

        TVector<T, N> getSize() const { return maxEdge - minEdge; }

        TVector<T, N> getCenter() const { return minEdge + ((maxEdge - minEdge) / T(2)); }

        bool isValid() const {
            for (int i = 0; i < N; i++) {
                if (minEdge[i] > maxEdge[i]) {
                    return false;
                }
            }

            return true;
        }

        bool isInside(const TVector<T, N> &point) const {
            for (int i = 0; i < N; ++i) {
                const T value = point[i];

                if (value < minEdge[i] || value > maxEdge[i]) {
                    return false;
                }
            }

            return true;
        }

        TVector<T, N> getEdge(int pointIndex) const {
            assert(pointIndex >= MinEdge);
            assert(pointIndex <= MaxEdge);

            if (pointIndex == MinEdge) {
                return minEdge;
            }

            if (pointIndex == MaxEdge) {
                return maxEdge;
            }

            auto edges = &minEdge;

            TVector<T, N> point;
            for (int i = 0; i < N; ++i) {
                const int remainder = pointIndex % 2;
                pointIndex /= 2;
                point[i] = edges[remainder][i];
            }

            return point;
        }

        /**
         * @brief Generate all the Normals in the Boundary
         *
         * @note Only implemented for N=2 and N=3 cases.
         *
         * @return std::array<TVector<T, N>, SideCount>
         */
        std::array<TVector<T, N>, SideCount> getNormals() const {
            if constexpr (N == 2) {
                return {Vector2<T>{static_cast<T>(-1), static_cast<T>(0)}, Vector2<T>{static_cast<T>(0), static_cast<T>(-1)}, Vector2<T>{static_cast<T>(1), static_cast<T>(0)},
                        Vector2<T>{static_cast<T>(0), static_cast<T>(1)}};
            }

            if constexpr (N == 3) {
                return {Vector3<T>{static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)}, Vector3<T>{static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)},
                        Vector3<T>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)}, Vector3<T>{static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)},
                        Vector3<T>{static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)},  Vector3<T>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)}};
            }
        }

        /**
         * @brief Perform an intersection test between two Boundaries using the Separating Axis Therorem (SAT).
         *
         * @param other
         * @return true
         * @return false
         */
        bool intersect(const Boundary<T, N> &other) const {
            // axes are the same with both shapes
            const auto axes = getNormals();

            for (const auto &axis : axes) {
                const Range<T> proj1 = project(axis);
                const Range<T> proj2 = other.project(axis);

                if (!proj1.overlap(proj2)) {
                    return false;
                }
            }

            return true;
        }

        /**
         * @brief Get all the Edges (vertices) from the Box.
         *
         * @return std::array<TVector<T, N>, PointCount>
         */
        std::array<TVector<T, N>, PointCount> getEdges() const {
            std::array<TVector<T, N>, PointCount> result;

            for (int i = 0; i < PointCount; i++) {
                result[i] = getEdge(i);
            }

            return result;
        }

        /**
         * @brief Perform a Projection from the specified Axis
         *
         * @note Utility method to support the implementation of the Intersect method.
         *
         * @return Projection
         */
        Range<T> project(const TVector<T, N> &normal) const {
            const auto edges = getEdges();

            Range<T> range{dot(edges[0], normal)};

            // compute the range for the rest
            for (std::size_t i = 1; i < edges.size(); i++) {
                const T value = dot(edges[i], normal);

                range.expand(value);
            }

            return range;
        }

    private:
        TVector<T, N> minEdge;
        TVector<T, N> maxEdge;
    };
} // namespace XE

#endif
