#pragma once

#include "Vector.h"

namespace xe {
    /**
     * @brief Ray in 3-space — origin + direction.
     *
     * Not part of glm. Field names follow the glm-ish convention: @c origin and
     * @c direction (instead of the old @c position + @c direction).
     */
    template <typename T> struct tray {
        tvec<T, 3> origin{T(0), T(0), T(0)};
        tvec<T, 3> direction{T(0), T(0), T(1)};

        constexpr tray() noexcept = default;

        constexpr tray(const tvec<T, 3> &o, const tvec<T, 3> &d) noexcept : origin(o), direction(d) {
        }

        //! Point along the ray at parameter @p t from the origin.
        [[nodiscard]] constexpr tvec<T, 3> pointAt(T t) const noexcept {
            return origin + t * direction;
        }
    };

    using ray = tray<float>;
    using dray = tray<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TRay = tray<T>;
    using Ray = ray;
    using Rayd = dray;
} // namespace xe
