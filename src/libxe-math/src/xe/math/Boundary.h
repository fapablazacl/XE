#pragma once

#include <array>
#include <cassert>
#include <cstddef>

#include "Range.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-aligned N-dimensional boundary (N=2 for rect, N=3 for box).
     *
     * Not part of glm — xe-specific utility used by @ref tboundary / @ref tbox / @ref trect.
     */
    template <typename T, int N> class tboundary {
        static_assert(N == 2 || N == 3, "tboundary only supports N in {2, 3}");

    public:
        template <int Base, int Exp> struct Power {
            static constexpr int value = Base * Power<Base, Exp - 1>::value;
        };

        template <int Base> struct Power<Base, 0> {
            static constexpr int value = 1;
        };

        static constexpr int SideCount = 2 * N;
        static constexpr int PointCount = Power<2, N>::value;
        static constexpr int MinEdgeIndex = 0;
        static constexpr int MaxEdgeIndex = PointCount - 1;

        constexpr tboundary() noexcept = default;

        constexpr explicit tboundary(const tvec<T, N> &value) noexcept : minEdge(value), maxEdge(value) {
        }

        constexpr tboundary(const tvec<T, N> &a, const tvec<T, N> &b) noexcept : tboundary(a) {
            expand(b);
        }

        constexpr void expand(const tvec<T, N> &value) noexcept {
            minEdge = min(minEdge, value);
            maxEdge = max(maxEdge, value);
        }

        constexpr void expand(const tboundary<T, N> &other) noexcept {
            expand(other.minEdge);
            expand(other.maxEdge);
        }

        [[nodiscard]] constexpr tvec<T, N> getMinEdge() const noexcept {
            return minEdge;
        }

        [[nodiscard]] constexpr tvec<T, N> getMaxEdge() const noexcept {
            return maxEdge;
        }

        [[nodiscard]] constexpr tvec<T, N> getSize() const noexcept {
            return maxEdge - minEdge;
        }

        [[nodiscard]] constexpr tvec<T, N> getCenter() const noexcept {
            return minEdge + ((maxEdge - minEdge) / T(2));
        }

        [[nodiscard]] constexpr bool isValid() const noexcept {
            for (int i = 0; i < N; ++i) {
                if (minEdge[i] > maxEdge[i]) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] constexpr bool isInside(const tvec<T, N> &point) const noexcept {
            for (int i = 0; i < N; ++i) {
                if (point[i] < minEdge[i] || point[i] > maxEdge[i]) {
                    return false;
                }
            }
            return true;
        }

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
         * @brief Separating-Axis-Theorem overlap test against another boundary.
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

        [[nodiscard]] std::array<tvec<T, N>, PointCount> getEdges() const {
            std::array<tvec<T, N>, PointCount> result;
            for (int i = 0; i < PointCount; ++i) {
                result[i] = getEdge(i);
            }
            return result;
        }

        [[nodiscard]] trange<T> project(const tvec<T, N> &normal) const {
            const auto edges = getEdges();
            trange<T> r{dot(edges[0], normal)};
            for (std::size_t i = 1; i < edges.size(); ++i) {
                r.expand(dot(edges[i], normal));
            }
            return r;
        }

    private:
        tvec<T, N> minEdge{};
        tvec<T, N> maxEdge{};
    };
} // namespace xe
