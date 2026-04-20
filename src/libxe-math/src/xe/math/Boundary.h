/**
 * @file Boundary.h
 * @brief N-dimensional axis-aligned bounding region — base of @ref tbox / @ref trect.
 *
 * The @ref xe::tboundary<T,N> class template is xe-specific (not part of
 * glm) and is the foundation for the engine's collision and visibility
 * tests. It is parameterised on the scalar type and the dimensionality
 * (only @c N=2 and @c N=3 are supported), and supplies:
 *   - growth via @ref tboundary::expand
 *   - corner / edge accessors used for SAT
 *   - axis-aligned face normals
 *   - separating-axis-theorem overlap testing through
 *     @ref tboundary::intersect / @ref tboundary::project
 */

#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include "Range.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-aligned N-dimensional boundary (N=2 → rectangle, N=3 → box).
     * Stores a min and max corner; everything else (size, center, corner
     * enumeration, face normals) is derived. Not part of glm — this is the
     * xe-specific bounding-volume primitive shared by @ref tbox and
     * @ref trect, and it is the type the SAT-based @ref intersect routine
     * operates on.
     */
    template <typename T, int N> class tboundary {
        static_assert(N == 2 || N == 3, "tboundary only supports N in {2, 3}");

    public:
        /**
         * @brief Compile-time integer power helper used to derive @ref PointCount.
         * Recursive metafunction that computes @c Base^Exp at compile time.
         * Lives inside @ref tboundary because it has no other consumer.
         */
        template <int Base, int Exp> struct Power {
            static constexpr int value = Base * Power<Base, Exp - 1>::value; ///< Recursive case.
        };

        /**
         * @brief Base case of the @ref Power recursion (@c Base^0 == 1).
         */
        template <int Base> struct Power<Base, 0> {
            static constexpr int value = 1; ///< Terminating value.
        };

        static constexpr int SideCount = 2 * N;               ///< Number of faces (4 for 2D, 6 for 3D).
        static constexpr int PointCount = Power<2, N>::value; ///< Number of corners (4 for 2D, 8 for 3D).
        static constexpr int MinEdgeIndex = 0;                ///< Corner index of @ref minEdge.
        static constexpr int MaxEdgeIndex = PointCount - 1;   ///< Corner index of @ref maxEdge.

        /**
         * @brief Default-construct an empty boundary at the origin.
         * Both corners are zero-initialised, so the resulting boundary is
         * a degenerate point at the origin. Call @ref expand to grow it.
         */
        constexpr tboundary() noexcept = default;

        /**
         * @brief Construct a degenerate boundary that contains a single point.
         * Use this together with @ref expand when building a boundary
         * incrementally from a stream of points.
         * @param value Point used for both the min and max corners.
         */
        constexpr explicit tboundary(const tvec<T, N> &value) noexcept : minEdge(value), maxEdge(value) {
        }

        /**
         * @brief Construct the smallest boundary that contains two points.
         * The points may be supplied in any order; the constructor takes
         * the per-axis minimum and maximum.
         * @param a First point.
         * @param b Second point.
         */
        constexpr tboundary(const tvec<T, N> &a, const tvec<T, N> &b) noexcept : tboundary(a) {
            expand(b);
        }

        /**
         * @brief Grow the boundary so that it contains @p value.
         * Updates the min and max corners with the per-axis minimum and
         * maximum of the current corners and @p value. Cheap and intended
         * to be called in tight loops while building a bounding volume
         * around a vertex stream.
         * @param value Point that must end up inside the boundary.
         */
        constexpr void expand(const tvec<T, N> &value) noexcept {
            minEdge = min(minEdge, value);
            maxEdge = max(maxEdge, value);
        }

        /**
         * @brief Grow the boundary so that it contains another boundary.
         * Equivalent to expanding by both corners of @p other in turn —
         * the cheapest way to merge two bounding volumes.
         * @param other Boundary that must end up inside @c *this.
         */
        constexpr void expand(const tboundary<T, N> &other) noexcept {
            expand(other.minEdge);
            expand(other.maxEdge);
        }

        /**
         * @brief Get the minimum-corner edge.
         * @return The per-axis-minimum corner of the boundary.
         */
        [[nodiscard]] constexpr tvec<T, N> getMinEdge() const noexcept {
            return minEdge;
        }

        /**
         * @brief Get the maximum-corner edge.
         * @return The per-axis-maximum corner of the boundary.
         */
        [[nodiscard]] constexpr tvec<T, N> getMaxEdge() const noexcept {
            return maxEdge;
        }

        /**
         * @brief Get the per-axis size of the boundary.
         * @return The vector @c maxEdge - minEdge.
         */
        [[nodiscard]] constexpr tvec<T, N> getSize() const noexcept {
            return maxEdge - minEdge;
        }

        /**
         * @brief Get the geometric centre (centroid) of the boundary.
         * @return The midpoint of @c minEdge and @c maxEdge.
         */
        [[nodiscard]] constexpr tvec<T, N> getCenter() const noexcept {
            return minEdge + ((maxEdge - minEdge) / T(2));
        }

        /**
         * @brief Check whether the boundary's corners are still well-ordered.
         * A boundary is valid when @c minEdge[i] @c <= @c maxEdge[i] for
         * every axis. Boundaries that fail this test arise from
         * uninitialised data or from incorrect expansion logic and should
         * be treated as bugs.
         * @return @c true when every component of @c minEdge is at most the matching component of @c maxEdge.
         */
        [[nodiscard]] constexpr bool isValid() const noexcept {
            for (int i = 0; i < N; ++i) {
                if (minEdge[i] > maxEdge[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Test whether a point lies inside the boundary, edges inclusive.
         * Returns @c true for points on a face, an edge, or a corner.
         * @param point Point to test.
         * @return @c true when @p point lies inside or on the boundary.
         */
        [[nodiscard]] constexpr bool isInside(const tvec<T, N> &point) const noexcept {
            for (int i = 0; i < N; ++i) {
                if (point[i] < minEdge[i] || point[i] > maxEdge[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Get one of the @ref PointCount corners by index.
         * Indices @c 0 and @c PointCount-1 always return the min and max
         * corners directly. Intermediate indices are decoded as a binary
         * mask: bit @c i selects @c minEdge[i] or @c maxEdge[i] for that
         * axis. The result is the standard enumeration of an N-dimensional
         * box's corners.
         * @param pointIndex Corner index in @c [0, PointCount).
         * @return The corresponding corner of the boundary.
         */
        [[nodiscard]] tvec<T, N> getEdge(int pointIndex) const {
            assert(pointIndex >= MinEdgeIndex);
            assert(pointIndex <= MaxEdgeIndex);

            if (pointIndex == MinEdgeIndex) {
                return minEdge;
            }
            if (pointIndex == MaxEdgeIndex) {
                return maxEdge;
            }

            const tvec<T, N> edges[2] = {minEdge, maxEdge};
            tvec<T, N> point;
            for (int i = 0; i < N; ++i) {
                const int remainder = pointIndex % 2;
                pointIndex /= 2;
                point[i] = edges[remainder][i];
            }
            return point;
        }

        /**
         * @brief Return the outward-facing axis-aligned face normals.
         * For 2D this is four normals (left, bottom, right, top); for 3D
         * it is six normals (the negative and positive x/y/z axes). The
         * order is fixed and is what @ref intersect uses as candidate
         * separating axes for SAT.
         * @return An array of @ref SideCount unit normals.
         */
        [[nodiscard]] std::array<tvec<T, N>, SideCount> getNormals() const {
            if constexpr (N == 2) {
                return {
                    tvec<T, 2>{static_cast<T>(-1), static_cast<T>(0)},
                    tvec<T, 2>{static_cast<T>(0), static_cast<T>(-1)},
                    tvec<T, 2>{static_cast<T>(1), static_cast<T>(0)},
                    tvec<T, 2>{static_cast<T>(0), static_cast<T>(1)},
                };
            } else {
                return {
                    tvec<T, 3>{static_cast<T>(-1), static_cast<T>(0), static_cast<T>(0)},
                    tvec<T, 3>{static_cast<T>(0), static_cast<T>(-1), static_cast<T>(0)},
                    tvec<T, 3>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(-1)},
                    tvec<T, 3>{static_cast<T>(1), static_cast<T>(0), static_cast<T>(0)},
                    tvec<T, 3>{static_cast<T>(0), static_cast<T>(1), static_cast<T>(0)},
                    tvec<T, 3>{static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)},
                };
            }
        }

        /**
         * @brief Test whether two axis-aligned boundaries overlap.
         * Implements the Separating Axis Theorem (SAT) using only the
         * face-normal axes (which is sufficient for AABBs because both
         * shapes share the same axis set). For each candidate axis the
         * routine projects both boundaries onto the axis with @ref project
         * and checks whether the resulting @ref trange intervals overlap.
         * If any axis separates them the boundaries are disjoint;
         * otherwise they intersect.
         * @param other The boundary to test against.
         * @return @c true when the two boundaries overlap (touching counts).
         */
        [[nodiscard]] bool intersect(const tboundary<T, N> &other) const {
            const auto axes = getNormals();
            for (const auto &axis : axes) {
                const trange<T> p1 = project(axis);
                const trange<T> p2 = other.project(axis);
                if (!p1.overlap(p2)) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Return all @ref PointCount corners of the boundary.
         * Helper used internally by @ref project and useful for callers
         * that need to render or transform the boundary's vertices
         * directly.
         * @return Array of @ref PointCount corner vectors.
         */
        [[nodiscard]] std::array<tvec<T, N>, PointCount> getEdges() const {
            std::array<tvec<T, N>, PointCount> result;
            for (int i = 0; i < PointCount; ++i) {
                result[i] = getEdge(i);
            }
            return result;
        }

        /**
         * @brief Project the boundary onto an arbitrary axis.
         * Computes the dot product of every corner against @p normal and
         * returns the resulting min/max scalar interval. This is the
         * second half of the SAT used by @ref intersect: a separating
         * axis exists when the projected intervals of the two shapes do
         * not overlap.
         * @param normal Axis to project onto; need not be unit-length.
         * @return The scalar @ref trange enclosing the projected corners.
         */
        [[nodiscard]] trange<T> project(const tvec<T, N> &normal) const {
            const auto edges = getEdges();
            trange<T> r{dot(edges[0], normal)};
            for (std::size_t i = 1; i < edges.size(); ++i) {
                r.expand(dot(edges[i], normal));
            }
            return r;
        }

    private:
        tvec<T, N> minEdge{}; ///< Per-axis minimum corner.
        tvec<T, N> maxEdge{}; ///< Per-axis maximum corner.
    };
} // namespace xe
