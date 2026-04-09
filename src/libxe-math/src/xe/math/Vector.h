/**
 * @file Vector.h
 * @brief GLM-style N-dimensional vector types for xe::math.
 *
 * Exposes @ref xe::tvec<T, N> together with the @c vec2/@c vec3/@c vec4,
 * @c ivec2-4, and @c dvec2-4 convenience aliases. Specializations for N=2,
 * N=3 and N=4 provide glm-compatible member access (@c .x/@c .y/@c .z/@c .w)
 * plus legacy uppercase aliases (@c .X/@c .Y/@c .Z/@c .W) for consumers still
 * on the old API — those will be removed in a follow-up.
 *
 * Layout is guaranteed contiguous so that @c data() and @c operator[] walk the
 * components in memory order (x, y, z, w). This matches glm::vec<N, T, glm::defaultp>
 * bit-for-bit for scalar T.
 */

#pragma once

// Windows headers define min/max as macros, which clash with xe::min and
// xe::max below. Undefine them pre-emptively so including <Windows.h> elsewhere
// in the translation unit doesn't break this file.
#if defined(max)
#undef max
#endif
#if defined(min)
#undef min
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ostream>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // nonstandard extension used: nameless struct/union
#endif

namespace xe {
    //! Primary template — generic N-dimensional vector used as a fallback.
    template <typename T, int N> struct tvec {
        T values[N] = {};

        constexpr tvec() noexcept = default;

        constexpr explicit tvec(T s) noexcept {
            for (int i = 0; i < N; ++i) {
                values[i] = s;
            }
        }

        constexpr explicit tvec(const T *ptr) noexcept {
            for (int i = 0; i < N; ++i) {
                values[i] = ptr[i];
            }
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return values;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return values;
        }

        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return values[i];
        }

        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return values[i];
        }
    };

    //! 2-component vector specialization.
    template <typename T> struct tvec<T, 2> {
        union {
            struct {
                T x, y;
            };
            // Legacy uppercase aliases — deprecated, kept for source compatibility.
            struct {
                T X, Y;
            };
        };

        constexpr tvec() noexcept : x(T{}), y(T{}) {
        }

        constexpr explicit tvec(T s) noexcept : x(s), y(s) {
        }

        constexpr tvec(T xv, T yv) noexcept : x(xv), y(yv) {
        }

        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]) {
        }

        //! Cross-type (implicit) construction — matches the old TVector conversion operator.
        template <typename U> constexpr tvec(const tvec<U, 2> &other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    //! 3-component vector specialization.
    template <typename T> struct tvec<T, 3> {
        union {
            struct {
                T x, y, z;
            };
            struct {
                T X, Y, Z;
            };
        };

        constexpr tvec() noexcept : x(T{}), y(T{}), z(T{}) {
        }

        constexpr explicit tvec(T s) noexcept : x(s), y(s), z(s) {
        }

        constexpr tvec(T xv, T yv, T zv) noexcept : x(xv), y(yv), z(zv) {
        }

        constexpr tvec(const tvec<T, 2> &v, T zv) noexcept : x(v.x), y(v.y), z(zv) {
        }

        constexpr tvec(T xv, const tvec<T, 2> &v) noexcept : x(xv), y(v.x), z(v.y) {
        }

        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]) {
        }

        //! Cross-type (implicit) construction.
        template <typename U>
        constexpr tvec(const tvec<U, 3> &other) noexcept
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    //! 4-component vector specialization.
    template <typename T> struct tvec<T, 4> {
        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T X, Y, Z, W;
            };
        };

        constexpr tvec() noexcept : x(T{}), y(T{}), z(T{}), w(T{}) {
        }

        constexpr explicit tvec(T s) noexcept : x(s), y(s), z(s), w(s) {
        }

        constexpr tvec(T xv, T yv, T zv, T wv) noexcept : x(xv), y(yv), z(zv), w(wv) {
        }

        constexpr tvec(const tvec<T, 3> &v, T wv) noexcept : x(v.x), y(v.y), z(v.z), w(wv) {
        }

        constexpr tvec(T xv, const tvec<T, 3> &v) noexcept : x(xv), y(v.x), z(v.y), w(v.z) {
        }

        constexpr tvec(const tvec<T, 2> &v, T zv, T wv) noexcept : x(v.x), y(v.y), z(zv), w(wv) {
        }

        constexpr tvec(T xv, const tvec<T, 2> &v, T wv) noexcept : x(xv), y(v.x), z(v.y), w(wv) {
        }

        constexpr tvec(T xv, T yv, const tvec<T, 2> &v) noexcept : x(xv), y(yv), z(v.x), w(v.y) {
        }

        constexpr tvec(const tvec<T, 2> &a, const tvec<T, 2> &b) noexcept : x(a.x), y(a.y), z(b.x), w(b.y) {
        }

        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {
        }

        //! Cross-type (implicit) construction.
        template <typename U>
        constexpr tvec(const tvec<U, 4> &other) noexcept
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) {
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    // ---------------------------------------------------------------------
    // Unary & binary arithmetic operators (componentwise).
    // ---------------------------------------------------------------------

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator+(const tvec<T, N> &v) noexcept {
        return v;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator-(const tvec<T, N> &v) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = -v[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator+(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] + b[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator-(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] - b[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] * b[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator/(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] / b[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const tvec<T, N> &v, const T s) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = v[i] * s;
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const T s, const tvec<T, N> &v) noexcept {
        return v * s;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator/(const tvec<T, N> &v, const T s) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = v[i] / s;
        }
        return r;
    }

    // Compound assignments.

    template <typename T, int N> constexpr tvec<T, N> &operator+=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] += b[i];
        }
        return a;
    }

    template <typename T, int N> constexpr tvec<T, N> &operator-=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] -= b[i];
        }
        return a;
    }

    template <typename T, int N> constexpr tvec<T, N> &operator*=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] *= b[i];
        }
        return a;
    }

    template <typename T, int N> constexpr tvec<T, N> &operator/=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] /= b[i];
        }
        return a;
    }

    template <typename T, int N> constexpr tvec<T, N> &operator*=(tvec<T, N> &a, const T s) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] *= s;
        }
        return a;
    }

    template <typename T, int N> constexpr tvec<T, N> &operator/=(tvec<T, N> &a, const T s) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] /= s;
        }
        return a;
    }

    // Equality.

    template <typename T, int N> [[nodiscard]] constexpr bool operator==(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            if (!(a[i] == b[i])) {
                return false;
            }
        }
        return true;
    }

    template <typename T, int N> [[nodiscard]] constexpr bool operator!=(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return !(a == b);
    }

    // ---------------------------------------------------------------------
    // Geometric functions (glm-compatible names).
    // ---------------------------------------------------------------------

    template <typename T, int N> [[nodiscard]] constexpr T dot(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        T acc = T{};
        for (int i = 0; i < N; ++i) {
            acc += a[i] * b[i];
        }
        return acc;
    }

    template <typename T> [[nodiscard]] constexpr tvec<T, 3> cross(const tvec<T, 3> &a, const tvec<T, 3> &b) noexcept {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    template <typename T> [[nodiscard]] constexpr T cross(const tvec<T, 2> &a, const tvec<T, 2> &b) noexcept {
        return a.x * b.y - a.y * b.x;
    }

    //! Squared length (glm::length2).
    template <typename T, int N> [[nodiscard]] constexpr T length2(const tvec<T, N> &v) noexcept {
        return dot(v, v);
    }

    //! Euclidean length (glm::length).
    template <typename T, int N> [[nodiscard]] T length(const tvec<T, N> &v) {
        return static_cast<T>(std::sqrt(length2(v)));
    }

    template <typename T, int N> [[nodiscard]] tvec<T, N> normalize(const tvec<T, N> &v) {
        return v / length(v);
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> min(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] < b[i] ? a[i] : b[i];
        }
        return r;
    }

    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> max(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] > b[i] ? a[i] : b[i];
        }
        return r;
    }

    // Debug stream output.
    template <typename T, int N> inline std::ostream &operator<<(std::ostream &os, const tvec<T, N> &v) {
        os << "xe::tvec<" << N << ">{ ";
        for (int i = 0; i < N - 1; ++i) {
            os << v[i] << ", ";
        }
        os << v[N - 1] << " }";
        return os;
    }

    // ---------------------------------------------------------------------
    // Canonical type aliases (match glm names exactly).
    // ---------------------------------------------------------------------

    using vec2 = tvec<float, 2>;
    using vec3 = tvec<float, 3>;
    using vec4 = tvec<float, 4>;

    using ivec2 = tvec<std::int32_t, 2>;
    using ivec3 = tvec<std::int32_t, 3>;
    using ivec4 = tvec<std::int32_t, 4>;

    using dvec2 = tvec<double, 2>;
    using dvec3 = tvec<double, 3>;
    using dvec4 = tvec<double, 4>;

    using uvec2 = tvec<std::uint32_t, 2>;
    using uvec3 = tvec<std::uint32_t, 3>;
    using uvec4 = tvec<std::uint32_t, 4>;

    // ---------------------------------------------------------------------
    // Legacy PascalCase aliases (kept here so any header that only pulls in
    // Vector.h still sees the old names). See Legacy.h for the full set of
    // deprecated forwarders.
    // ---------------------------------------------------------------------

    template <typename T, int N> using TVector = tvec<T, N>;
    template <typename T> using TVector2 = tvec<T, 2>;
    template <typename T> using TVector3 = tvec<T, 3>;
    template <typename T> using TVector4 = tvec<T, 4>;

    using Vector2 = vec2;
    using Vector3 = vec3;
    using Vector4 = vec4;

    using Vector2i = ivec2;
    using Vector3i = ivec3;
    using Vector4i = ivec4;

    using Vector2d = dvec2;
    using Vector3d = dvec3;
    using Vector4d = dvec4;

    //! Legacy: xe::norm(v) — use xe::length(v).
    template <typename T, int N> [[nodiscard]] T norm(const tvec<T, N> &v) {
        return length(v);
    }

    //! Legacy: xe::norm2(v) — use xe::length2(v).
    template <typename T, int N> [[nodiscard]] constexpr T norm2(const tvec<T, N> &v) noexcept {
        return length2(v);
    }

    //! Legacy: xe::maximize(a, b) — use xe::max(a, b).
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> maximize(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return max(a, b);
    }

    //! Legacy: xe::minimize(a, b) — use xe::min(a, b).
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> minimize(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return min(a, b);
    }
} // namespace xe

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
