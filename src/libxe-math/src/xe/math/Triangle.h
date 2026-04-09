#pragma once

#include "Vector.h"

namespace xe {
    /**
     * @brief Triangle in 3-space. Not part of glm.
     */
    template <typename T> struct ttriangle {
        tvec<T, 3> p1{};
        tvec<T, 3> p2{};
        tvec<T, 3> p3{};

        constexpr ttriangle() noexcept = default;

        constexpr ttriangle(const tvec<T, 3> &a, const tvec<T, 3> &b, const tvec<T, 3> &c) noexcept : p1(a), p2(b), p3(c) {
        }

        [[nodiscard]] tvec<T, 3> computeNormal() const {
            return normalize(computeNormalUnnormalized());
        }

        [[nodiscard]] constexpr tvec<T, 3> computeNormalUnnormalized() const noexcept {
            return cross(p2 - p1, p3 - p1);
        }
    };

    using triangle = ttriangle<float>;
    using dtriangle = ttriangle<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TTriangle = ttriangle<T>;
    using Triangle = triangle;
    using Triangled = dtriangle;
} // namespace xe
