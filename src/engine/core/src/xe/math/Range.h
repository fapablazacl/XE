
#pragma once

#include <algorithm>
#include <cassert>
#include <ostream>

namespace XE {
    /**
     * @brief Mathematical Range pair implementation.
     *
     * This struct represent Ranges of the form:
     * [min, max)
     *
     * @tparam T
     */
    template <typename T> struct Range {
        T min = static_cast<T>(0);
        T max = static_cast<T>(1);

        Range() {}

        explicit Range(const T value) : min(value), max(value) {}

        explicit Range(const T value1, const T value2) : Range(value1) { expand(value2); }

        void expand(const T value) {
            min = std::min(min, value);
            max = std::max(max, value);
        }

        bool overlap(const Range<T> &other) const { return partialOverlap(other) || other.partialOverlap(*this); }

        /**
         * @brief Checks if the current Projection overlaps with the supplied Projection.
         *
         * @param other
         * @return true
         * @return false
         */
        bool partialOverlap(const Range<T> &other) const {
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
     * @brief Serializes a Range<T> using the supplied ostream
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const Range<T> &range) {
        os << "XE::Range<" << typeid(T).name() << ">{ ";

        os << range.min << ", ";
        os << range.max << " }";

        return os;
    }
} // namespace XE
