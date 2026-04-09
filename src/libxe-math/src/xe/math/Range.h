#pragma once

#include <algorithm>
#include <cassert>
#include <ostream>

namespace xe {
    /**
     * @brief Half-open numeric range @f$[min, max)@f$.
     *
     * Not part of glm — it's an xe::math utility used by @ref tboundary for SAT
     * projection intersection tests.
     */
    template <typename T> struct trange {
        T min = static_cast<T>(0);
        T max = static_cast<T>(1);

        constexpr trange() noexcept = default;

        constexpr explicit trange(T value) noexcept : min(value), max(value) {
        }

        constexpr trange(T a, T b) noexcept : min(a), max(a) {
            expand(b);
        }

        constexpr void expand(T value) noexcept {
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }

        [[nodiscard]] constexpr bool overlap(const trange<T> &other) const noexcept {
            return partialOverlap(other) || other.partialOverlap(*this);
        }

        [[nodiscard]] constexpr bool partialOverlap(const trange<T> &other) const noexcept {
            assert(max >= min);
            assert(other.max >= other.min);

            if (min >= other.min && min < other.max) {
                return true;
            }
            if (max > other.min && max < other.max) {
                return true;
            }
            return false;
        }
    };

    template <typename T> inline std::ostream &operator<<(std::ostream &os, const trange<T> &r) {
        os << "xe::trange{ " << r.min << ", " << r.max << " }";
        return os;
    }

    using range = trange<float>;
    using drange = trange<double>;
    using irange = trange<int>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TRange = trange<T>;
    using Range = range;
    using Ranged = drange;
    using Rangei = irange;
} // namespace xe
