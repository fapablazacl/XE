/**
 * @file Quaternion.h
 * @brief GLM-style quaternion type for xe::math.
 *
 * Matches glm::qua<T, Q>:
 *   - Memory layout is (x, y, z, w) — 16 bytes for float, bit-identical to glm::quat.
 *   - The default scalar-first constructor @c tquat(w, x, y, z) also matches glm.
 *   - @c length / @c length2 / @c normalize / @c dot / @c conjugate / @c inverse
 *     names line up with glm's.
 *   - Use @c angleAxis(radians, axis) to build a rotation (matches glm::angleAxis).
 *   - @c q * vec3 rotates the vector.
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

        constexpr tquat() noexcept : x(T{}), y(T{}), z(T{}), w(T(1)) {
        }

        //! Scalar-first constructor, matching glm::quat(w, x, y, z).
        constexpr tquat(T wv, T xv, T yv, T zv) noexcept : x(xv), y(yv), z(zv), w(wv) {
        }

        //! Build from a 3D vector (imaginary part) and a scalar.
        constexpr tquat(const tvec<T, 3> &v, T wv) noexcept : x(v.x), y(v.y), z(v.z), w(wv) {
        }

        //! Build from a pointer to four values in (x, y, z, w) order.
        constexpr explicit tquat(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {
        }

        //! Build from a 4-vector interpreted as (x, y, z, w).
        constexpr explicit tquat(const tvec<T, 4> &v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {
        }

        [[nodiscard]] static constexpr std::size_t size() noexcept {
            return 4;
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        [[nodiscard]] constexpr T &operator[](std::size_t i) noexcept {
            return (&x)[i];
        }

        [[nodiscard]] constexpr const T &operator[](std::size_t i) const noexcept {
            return (&x)[i];
        }

        //! Explicit conversion to an axis-angle rotation.
        explicit operator axis_angle<T>() const {
            const T a = T(2) * std::acos(w);
            if (a == T(0)) {
                return {a, {T(1), T(0), T(0)}};
            }
            return {a, normalize(tvec<T, 3>{x, y, z})};
        }

        //! Explicit conversion to a 4-vector (x, y, z, w).
        explicit constexpr operator tvec<T, 4>() const noexcept {
            return {x, y, z, w};
        }
    };

    // ---------------------------------------------------------------------
    // Arithmetic operators.
    // ---------------------------------------------------------------------

    template <typename T> [[nodiscard]] constexpr tquat<T> operator+(const tquat<T> &q) noexcept {
        return q;
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator-(const tquat<T> &q) noexcept {
        return {-q.w, -q.x, -q.y, -q.z};
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator+(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator-(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z};
    }

    //! Hamilton product — matches glm's quat * quat.
    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(const tquat<T> &a, const tquat<T> &b) noexcept {
        return {
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, // w
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y, // x
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, // y
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w, // z
        };
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(const tquat<T> &q, T s) noexcept {
        return {q.w * s, q.x * s, q.y * s, q.z * s};
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator*(T s, const tquat<T> &q) noexcept {
        return q * s;
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> operator/(const tquat<T> &q, T s) noexcept {
        return {q.w / s, q.x / s, q.y / s, q.z / s};
    }

    template <typename T> constexpr tquat<T> &operator+=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a + b;
        return a;
    }

    template <typename T> constexpr tquat<T> &operator-=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a - b;
        return a;
    }

    template <typename T> constexpr tquat<T> &operator*=(tquat<T> &a, const tquat<T> &b) noexcept {
        a = a * b;
        return a;
    }

    template <typename T> constexpr tquat<T> &operator*=(tquat<T> &q, T s) noexcept {
        q = q * s;
        return q;
    }

    template <typename T> constexpr tquat<T> &operator/=(tquat<T> &q, T s) noexcept {
        q = q / s;
        return q;
    }

    template <typename T> [[nodiscard]] constexpr bool operator==(const tquat<T> &a, const tquat<T> &b) noexcept {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    template <typename T> [[nodiscard]] constexpr bool operator!=(const tquat<T> &a, const tquat<T> &b) noexcept {
        return !(a == b);
    }

    // ---------------------------------------------------------------------
    // Geometric functions.
    // ---------------------------------------------------------------------

    template <typename T> [[nodiscard]] constexpr T dot(const tquat<T> &a, const tquat<T> &b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    template <typename T> [[nodiscard]] constexpr T length2(const tquat<T> &q) noexcept {
        return dot(q, q);
    }

    template <typename T> [[nodiscard]] T length(const tquat<T> &q) {
        return static_cast<T>(std::sqrt(length2(q)));
    }

    template <typename T> [[nodiscard]] tquat<T> normalize(const tquat<T> &q) {
        return q / length(q);
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> conjugate(const tquat<T> &q) noexcept {
        return {q.w, -q.x, -q.y, -q.z};
    }

    template <typename T> [[nodiscard]] constexpr tquat<T> inverse(const tquat<T> &q) noexcept {
        return conjugate(q) / length2(q);
    }

    /**
     * @brief Rotate a 3D vector by a unit quaternion. Matches @c glm::quat * vec3.
     */
    template <typename T> [[nodiscard]] constexpr tvec<T, 3> operator*(const tquat<T> &q, const tvec<T, 3> &v) noexcept {
        const tvec<T, 3> qv{q.x, q.y, q.z};
        const tvec<T, 3> t = T(2) * cross(qv, v);
        return v + q.w * t + cross(qv, t);
    }

    /**
     * @brief Build a unit quaternion from an (angle, axis) pair — matches glm::angleAxis.
     * @param radians  Rotation angle in radians.
     * @param axis     Unit-length rotation axis.
     */
    template <typename T> [[nodiscard]] tquat<T> angleAxis(T radians, const tvec<T, 3> &axis) {
        const T half = radians * T(0.5);
        const T s = std::sin(half);
        return {std::cos(half), axis.x * s, axis.y * s, axis.z * s};
    }

    // ---------------------------------------------------------------------
    // Type aliases.
    // ---------------------------------------------------------------------

    using quat = tquat<float>;
    using dquat = tquat<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TQuaternion = tquat<T>;
    using Quat = quat;
    using Quatd = dquat;
} // namespace xe

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
