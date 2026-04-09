/**
 * @file Range.h
 * @brief Half-open scalar range used by SAT projection.
 */

#pragma once

#include <algorithm>
#include <cassert>
#include <ostream>

namespace xe {
    /**
     * @brief Half-open numeric range @f$[\min, \max)@f$. Not part of glm.
     * Used by @ref tboundary as the result of @ref tboundary::project for
     * Separating-Axis-Theorem overlap testing. The interval is half-open:
     * the lower bound is inclusive and the upper bound is exclusive,
     * matching how scalar projection bounds compose under SAT.
     */
    template <typename T> struct trange {
        T min = static_cast<T>(0); ///< Inclusive lower bound.
        T max = static_cast<T>(1); ///< Exclusive upper bound.

        /**
         * @brief Default-construct to the canonical @c [0, 1) range.
         */
        constexpr trange() noexcept = default;

        /**
         * @brief Construct a degenerate range that contains a single value.
         * Useful as the seed for an incremental range-build via @ref expand.
         * @param value Value used for both bounds.
         */
        constexpr explicit trange(T value) noexcept : min(value), max(value) {
        }

        /**
         * @brief Construct the smallest range that contains both @p a and @p b.
         * The arguments may be supplied in any order; @ref expand handles
         * the ordering.
         * @param a First value.
         * @param b Second value.
         */
        constexpr trange(T a, T b) noexcept : min(a), max(a) {
            expand(b);
        }

        /**
         * @brief Grow the range to contain @p value.
         * Updates @c min or @c max as needed; cheap and intended to be
         * called in tight loops while accumulating a projection.
         * @param value Value that must end up inside the range.
         */
        constexpr void expand(T value) noexcept {
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }

        /**
         * @brief Test whether two ranges overlap.
         * Symmetrically combines @ref partialOverlap so the test works
         * regardless of which range starts first.
         * @param other The range to test against.
         * @return @c true when the two ranges share at least one half-open subinterval.
         */
        [[nodiscard]] constexpr bool overlap(const trange<T> &other) const noexcept {
            return partialOverlap(other) || other.partialOverlap(*this);
        }

        /**
         * @brief One-sided overlap test used by @ref overlap.
         * Returns @c true when either bound of @c *this lies inside the
         * half-open interval @c [other.min, other.max). Asserts that both
         * ranges are well-ordered.
         * @param other The range to test against.
         * @return @c true when @c *this has a bound inside @p other.
         */
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

    /**
     * @brief Stream insertion for @ref trange — debug print.
     * Writes the range in the form @c "xe::trange{ min, max }". Not
     * intended as a serialisation format.
     * @param os Output stream.
     * @param r Range to print.
     * @return The same stream, to allow chaining.
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const trange<T> &r) {
        os << "xe::trange{ " << r.min << ", " << r.max << " }";
        return os;
    }

    using range = trange<float>;   ///< Single-precision range.
    using drange = trange<double>; ///< Double-precision range.
    using irange = trange<int>;    ///< Integer range.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TRange = trange<T>; ///< @deprecated Use @ref trange.
    using Range = range;   ///< @deprecated Use @ref range.
    using Ranged = drange; ///< @deprecated Use @ref drange.
    using Rangei = irange; ///< @deprecated Use @ref irange.
} // namespace xe
