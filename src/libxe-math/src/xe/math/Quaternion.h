/**
 * @file Quaternion.h
 * @brief GLM-style quaternion type for xe::math.
 *
 * Matches @c glm::qua<T, Q>:
 *   - Memory layout is @c (x, y, z, w) — 16 bytes for @c float, bit-identical to @c glm::quat.
 *   - The default scalar-first constructor @c tquat(w, x, y, z) also matches glm.
 *   - @c length / @c length2 / @c normalize / @c dot / @c conjugate / @c inverse
 *     names line up with glm's.
 *   - Use @c angleAxis(radians, axis) to build a rotation (matches @c glm::angleAxis).
 *   - @c q * vec3 rotates the vector.
 *
 * The unnamed-union with the legacy uppercase @c .X / @c .Y / @c .Z / @c .W
 * fields is kept for source compatibility with the pre-refactor API and is
 * deprecated.
 */

#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>

#include "Common.h"
#include "Rotation.h"
#include "Vector.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

namespace xe {
    /**
     * @brief Quaternion type used to represent 3D rotations.
     * Storage is @c (x, y, z, w) in memory — bit-for-bit compatible with
     * @c glm::qua<T, Q>, so a @ref tquat can be passed to any glm-aware API
     * without conversion. The legacy uppercase @c .X / @c .Y / @c .Z / @c .W
     * aliases live in a second anonymous struct of the same union and are
     * deprecated; new code should use the lowercase fields.
     *
     * Note: the scalar-first constructor @c tquat(w, x, y, z) (matching
     * glm) is the canonical way to build a quaternion from raw components,
     * but the @ref angleAxis free function is what most call sites should
     * actually reach for.
     */
    template <typename T> struct tquat {
        // Storage: x, y, z, w in memory — matches glm::qua<T, Q>.
        // The legacy uppercase X/Y/Z/W aliases live in a second anon struct
        // of the same union, kept for source compatibility with consumer code
        // that still uses `q.W` — see Legacy.h.
        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T X, Y, Z, W;
            };
        };

        /**
         * @brief Default-construct to the identity quaternion @c (0, 0, 0, 1).
         * The identity represents no rotation; rotating any vector by it
         * yields the same vector unchanged.
         */
        constexpr tquat() noexcept : x(T{}), y(T{}), z(T{}), w(T(1)) {
        }

        /**
         * @brief Scalar-first component constructor, matching @c glm::quat(w, x, y, z).
         * Note the parameter order: @p wv comes first even though it lives
         * last in memory. This mirrors glm so call sites can be ported
         * without rewriting argument order.
         * @param wv Real (scalar) part.
         * @param xv First imaginary component.
         * @param yv Second imaginary component.
         * @param zv Third imaginary component.
         */
        constexpr tquat(T wv, T xv, T yv, T zv) noexcept : x(xv), y(yv), z(zv), w(wv) {
        }

        /**
         * @brief Build from a 3-vector imaginary part and a scalar real part.
         * @param v Imaginary part @c (x, y, z).
         * @param wv Real (scalar) part.
         */
        constexpr tquat(const tvec<T, 3> &v, T wv) noexcept : x(v.x), y(v.y), z(v.z), w(wv) {
        }

        /**
         * @brief Build from a pointer to four values in @c (x, y, z, w) memory order.
         * @param ptr Pointer to at least four readable @p T values.
         */
        constexpr explicit tquat(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {
        }

        /**
         * @brief Build from a 4-vector interpreted as @c (x, y, z, w).
         * Made @c explicit because reinterpreting an arbitrary vector as a
         * quaternion is rarely what the caller wants by accident.
         * @param v Source vector.
         */
        constexpr explicit tquat(const tvec<T, 4> &v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {
        }

        /**
         * @brief Number of scalar components in a quaternion.
         * Always 4. Provided for symmetry with vector-style generic code.
         * @return The constant @c 4.
         */
        [[nodiscard]] static constexpr std::size_t size() noexcept {
            return 4;
        }

        /**
         * @brief Mutable pointer to the first component (@c x).
         * @return Pointer that walks @c x, @c y, @c z, @c w in memory order.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        /**
         * @brief Const pointer to the first component (@c x).
         * @return Const pointer that walks @c x, @c y, @c z, @c w in memory order.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        /**
         * @brief Component access by index (0=x, 1=y, 2=z, 3=w).
         * @param i Component index in @c [0, 4).
         * @return Mutable reference to the requested component.
         */
        [[nodiscard]] constexpr T &operator[](std::size_t i) noexcept {
            return (&x)[i];
        }

        /**
         * @brief Const component access by index (0=x, 1=y, 2=z, 3=w).
         * @param i Component index in @c [0, 4).
         * @return Const reference to the requested component.
         */
        [[nodiscard]] constexpr const T &operator[](std::size_t i) const noexcept {
            return (&x)[i];
        }

        /**
         * @brief Explicit conversion to an axis-angle rotation.
         * Useful when interoperating with APIs that expect angle and axis
         * separately. The angle is in radians; when the quaternion is the
         * identity (no rotation), the axis defaults to @c (1, 0, 0) so the
         * caller still receives a valid pair.
         * @return The equivalent @ref axis_angle representation.
         */
        explicit operator axis_angle<T>() const {
            const T a = T(2) * std::acos(w);
            if (a == T(0)) {
                return {a, {T(1), T(0), T(0)}};
            }
            return {a, normalize(tvec<T, 3>{x, y, z})};
        }

        /**
         * @brief Explicit conversion to a 4-vector @c (x, y, z, w).
         * @return The quaternion's components as a @ref tvec<T,4>.
         */
        explicit constexpr operator tvec<T, 4>() const noexcept {
            return {x, y, z, w};
        }
    };

    // ---------------------------------------------------------------------
    // Arithmetic operators.
    // ---------------------------------------------------------------------

    /**
     * @brief Unary plus — returns a copy of @p q unchanged.
     * @param q Quaternion to copy.
     * @return A copy of @p q.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator+(const tquat<T> &q) noexcept {
        return q;
    }

    /**
     * @brief Componentwise negation.
     * Note: a quaternion and its negation represent the same rotation, but
     * they differ as numerical values.
     * @param q Quaternion to negate.
     * @return The componentwise negated quaternion.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator-(const tquat<T> &q) noexcept {
        return {-q.w, -q.x, -q.y, -q.z};
    }

    /**
     * @brief Componentwise quaternion addition.
     * @param a Left operand.
     * @param b Right operand.
     * @return The componentwise sum.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator+(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
    }

    /**
     * @brief Componentwise quaternion subtraction.
     * @param a Left operand.
     * @param b Right operand.
     * @return The componentwise difference.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator-(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z};
    }

    /**
     * @brief Quaternion-quaternion product (Hamilton product).
     * Matches glm's @c quat @c * @c quat. Composition is read right-to-left:
     * @c a*b applied to a vector first applies @p b then applies @p a, just
     * like matrix composition. This operation is non-commutative.
     * @param a Left operand.
     * @param b Right operand.
     * @return The Hamilton product @c a * b.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, // w
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y, // x
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, // y
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w, // z
        };
    }

    /**
     * @brief Quaternion-by-scalar multiplication.
     * @param q Quaternion operand.
     * @param s Scalar operand.
     * @return The componentwise product.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(const tquat<T> &q, T s) noexcept {
        return {q.w * s, q.x * s, q.y * s, q.z * s};
    }

    /**
     * @brief Scalar-by-quaternion multiplication (commutative form).
     * @param s Scalar operand.
     * @param q Quaternion operand.
     * @return The componentwise product.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(T s, const tquat<T> &q) noexcept {
        return q * s;
    }

    /**
     * @brief Quaternion-by-scalar division.
     * @param q Quaternion operand.
     * @param s Scalar divisor; behaviour is undefined when zero.
     * @return The componentwise quotient.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> operator/(const tquat<T> &q, T s) noexcept {
        return {q.w / s, q.x / s, q.y / s, q.z / s};
    }

    /**
     * @brief In-place componentwise quaternion addition.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T> constexpr tquat<T> &operator+=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a + b;
        return a;
    }

    /**
     * @brief In-place componentwise quaternion subtraction.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T> constexpr tquat<T> &operator-=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a - b;
        return a;
    }

    /**
     * @brief In-place Hamilton product.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T> constexpr tquat<T> &operator*=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a * b;
        return a;
    }

    /**
     * @brief In-place scalar multiplication.
     * @param q Left operand, modified in place.
     * @param s Scalar multiplier.
     * @return Reference to @p q after the assignment.
     */
    template <typename T> constexpr tquat<T> &operator*=(tquat<T> &q, T s) noexcept {
        q = q * s;
        return q;
    }

    /**
     * @brief In-place scalar division.
     * @param q Left operand, modified in place.
     * @param s Scalar divisor; behaviour is undefined when zero.
     * @return Reference to @p q after the assignment.
     */
    template <typename T> constexpr tquat<T> &operator/=(tquat<T> &q, T s) noexcept {
        q = q / s;
        return q;
    }

    /**
     * @brief Strict componentwise equality comparison.
     * Note that @c -q and @c q represent the same rotation but compare as
     * unequal here.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when every component matches exactly.
     */
    template <typename T> [[nodiscard]] constexpr bool operator==(const tquat<T> &a, const tquat<T> &b) noexcept {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    /**
     * @brief Strict componentwise inequality comparison.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when at least one component differs.
     */
    template <typename T> [[nodiscard]] constexpr bool operator!=(const tquat<T> &a, const tquat<T> &b) noexcept {
        return !(a == b);
    }

    // ---------------------------------------------------------------------
    // Geometric functions.
    // ---------------------------------------------------------------------

    /**
     * @brief Quaternion dot product.
     * Treats the two quaternions as 4-vectors and returns their inner
     * product. The dot product of unit quaternions equals the cosine of
     * half the angle between the rotations they represent.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w.
     */
    template <typename T> [[nodiscard]] constexpr T dot(const tquat<T> &a, const tquat<T> &b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    /**
     * @brief Squared magnitude of a quaternion.
     * Cheaper than @ref length because it skips the square root, useful
     * when only ordering matters.
     * @param q Input quaternion.
     * @return @c dot(q, q).
     */
    template <typename T> [[nodiscard]] constexpr T length2(const tquat<T> &q) noexcept {
        return dot(q, q);
    }

    /**
     * @brief Euclidean magnitude of a quaternion.
     * Calls @c std::sqrt and is therefore not @c constexpr in C++17.
     * Unit quaternions (those representing pure rotations) have length 1.
     * @param q Input quaternion.
     * @return @c sqrt(length2(q)).
     */
    template <typename T> [[nodiscard]] T length(const tquat<T> &q) {
        return static_cast<T>(std::sqrt(length2(q)));
    }

    /**
     * @brief Return the unit-length version of @p q.
     * Behaviour is undefined when @p q is the zero quaternion. Quaternions
     * accumulated through long chains of multiplication tend to drift away
     * from unit length, so callers should normalize periodically.
     * @param q Input quaternion; should be non-zero.
     * @return A quaternion parallel to @p q with length 1.
     */
    template <typename T> [[nodiscard]] tquat<T> normalize(const tquat<T> &q) {
        return q / length(q);
    }

    /**
     * @brief Return the conjugate of a quaternion.
     * Negates the imaginary part while leaving the real part untouched.
     * For unit quaternions the conjugate equals the inverse and represents
     * the inverse rotation.
     * @param q Input quaternion.
     * @return @c (w, -x, -y, -z).
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> conjugate(const tquat<T> &q) noexcept {
        return {q.w, -q.x, -q.y, -q.z};
    }

    /**
     * @brief Return the multiplicative inverse of a quaternion.
     * Equal to @c conjugate(q) @c / @c length2(q). For unit quaternions the
     * inverse coincides with the conjugate; the explicit division here
     * makes the function correct even for non-unit inputs.
     * @param q Input quaternion; must be non-zero.
     * @return The quaternion @c q' such that @c q*q' = q'*q = identity.
     */
    template <typename T> [[nodiscard]] constexpr tquat<T> inverse(const tquat<T> &q) noexcept {
        return conjugate(q) / length2(q);
    }

    /**
     * @brief Rotate a 3D vector by a unit quaternion. Matches @c glm::quat @c * @c vec3.
     * Uses the optimised cross-product form (no full quaternion multiply)
     * so the cost is two cross products and a few adds. The quaternion is
     * assumed to be unit length; results for non-unit quaternions are
     * undefined.
     * @param q Unit quaternion representing the rotation.
     * @param v Vector to rotate.
     * @return The rotated vector.
     */
    template <typename T> [[nodiscard]] constexpr tvec<T, 3> operator*(const tquat<T> &q, const tvec<T, 3> &v) noexcept {
        const tvec<T, 3> qv{q.x, q.y, q.z};
        const tvec<T, 3> t = T(2) * cross(qv, v);
        return v + q.w * t + cross(qv, t);
    }

    /**
     * @brief Build a unit quaternion from an angle-and-axis pair.
     * Matches @c glm::angleAxis. The axis is assumed to be already
     * normalised; if it is not, the resulting quaternion will not be unit
     * length and rotation operations will behave incorrectly. Caller is
     * responsible for normalising the axis up front.
     * @param radians Rotation angle in radians.
     * @param axis Unit-length rotation axis.
     * @return A unit quaternion representing the rotation.
     */
    template <typename T> [[nodiscard]] tquat<T> angleAxis(T radians, const tvec<T, 3> &axis) {
        const T half = radians * T(0.5);
        const T s = std::sin(half);
        return {std::cos(half), axis.x * s, axis.y * s, axis.z * s};
    }

    // ---------------------------------------------------------------------
    // Type aliases.
    // ---------------------------------------------------------------------

    using quat = tquat<float>;   ///< Single-precision quaternion — glm-compatible alias.
    using dquat = tquat<double>; ///< Double-precision quaternion.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TQuaternion = tquat<T>; ///< @deprecated Use @ref tquat.
    using Quat = quat;                                  ///< @deprecated Use @ref quat.
    using Quatd = dquat;                                ///< @deprecated Use @ref dquat.
} // namespace xe

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
