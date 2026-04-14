/**
 * @file Triangle.h
 * @brief 3D triangle primitive with normal computation.
 */

#pragma once

#include "Vector.h"

namespace xe {
    /**
     * @brief Triangle in 3-space — three vertices in fixed winding order. Not part of glm.
     * The vertex order matters for normal computation: the right-handed
     * cross product @c (p2 - p1) @c × @c (p3 - p1) determines which side
     * of the triangle is the "front". Reversing two vertices flips the
     * normal.
     */
    template <typename T> struct ttriangle {
        tvec<T, 3> p1{}; ///< First vertex.
        tvec<T, 3> p2{}; ///< Second vertex.
        tvec<T, 3> p3{}; ///< Third vertex.

        /**
         * @brief Default-construct to a degenerate triangle at the origin.
         */
        constexpr ttriangle() noexcept = default;

        /**
         * @brief Construct from three explicit vertices.
         * @param a First vertex.
         * @param b Second vertex.
         * @param c Third vertex.
         */
        constexpr ttriangle(const tvec<T, 3> &a, const tvec<T, 3> &b, const tvec<T, 3> &c) noexcept : p1(a), p2(b), p3(c) {
        }

        /**
         * @brief Compute the unit-length face normal of the triangle.
         * Uses @ref computeNormalUnnormalized and then normalises. Calls
         * @c std::sqrt and is therefore not @c constexpr.
         * @return The unit normal in the right-handed sense from the triangle's winding.
         */
        [[nodiscard]] tvec<T, 3> computeNormal() const {
            return normalize(computeNormalUnnormalized());
        }

        /**
         * @brief Compute the un-normalised face normal of the triangle.
         * Returns the cross product @c (p2 - p1) @c × @c (p3 - p1). The
         * magnitude of the result equals twice the triangle's area, which
         * makes this useful for area-weighted normal averaging across a
         * mesh — that is the main reason the un-normalised form is exposed.
         * @return The cross product of two edge vectors.
         */
        [[nodiscard]] constexpr tvec<T, 3> computeNormalUnnormalized() const noexcept {
            return cross(p2 - p1, p3 - p1);
        }
    };

    using triangle = ttriangle<float>;   ///< Single-precision triangle.
    using dtriangle = ttriangle<double>; ///< Double-precision triangle.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TTriangle = ttriangle<T>; ///< @deprecated Use @ref ttriangle.
    using Triangle = triangle;   ///< @deprecated Use @ref triangle.
    using Triangled = dtriangle; ///< @deprecated Use @ref dtriangle.
} // namespace xe
