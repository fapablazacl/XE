/**
 * @file Vector.h
 * @brief GLM-style N-dimensional vector types for xe::math.
 *
 * Exposes @ref xe::tvec<T, N> together with the @c vec2/@c vec3/@c vec4,
 * @c ivec2-4, @c uvec2-4 and @c dvec2-4 convenience aliases. Specializations
 * for @c N=2, @c N=3 and @c N=4 provide glm-compatible member access
 * (@c .x/@c .y/@c .z/@c .w) plus legacy uppercase aliases
 * (@c .X/@c .Y/@c .Z/@c .W) for consumers still on the old API — those
 * uppercase fields are deprecated and will be removed in a follow-up pass.
 *
 * Layout is guaranteed contiguous so that @c data() and @c operator[] walk
 * the components in memory order @c (x, y, z, w). This matches
 * @c glm::vec<N, T, glm::defaultp> bit-for-bit for scalar @p T, which means
 * a @c tvec instance can be uploaded to a GPU buffer or memcpy'd into a glm
 * struct without conversion. Every operation in this header is either
 * @c constexpr or @c noexcept (the only exceptions are @ref length and
 * @ref normalize, which call @c std::sqrt and therefore cannot be
 * @c constexpr in C++17).
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
    /**
     * @brief Generic N-dimensional vector — primary template / fallback storage.
     * Used whenever the dimension @p N is something other than 2, 3 or 4.
     * The components live in a plain C array so the layout stays trivially
     * copyable and the type can be passed straight to GPU APIs. Specialise
     * @ref tvec<T,2>, @ref tvec<T,3>, @ref tvec<T,4> when you need named
     * @c .x/@c .y/@c .z/@c .w accessors.
     */
    template <typename T, int N> struct tvec {
        T values[N] = {};

        /**
         * @brief Default-construct to the zero vector.
         * Components are value-initialised so the vector is deterministic
         * even before any assignment.
         */
        constexpr tvec() noexcept = default;

        /**
         * @brief Splat construct: copy the same scalar into every component.
         * Convenient for building uniform vectors such as @c tvec(1) for an
         * all-ones vector.
         * @param s Scalar to broadcast into each component.
         */
        constexpr explicit tvec(T s) noexcept {
            for (int i = 0; i < N; ++i) {
                values[i] = s;
            }
        }

        /**
         * @brief Construct from a contiguous buffer of @p N scalars.
         * The caller is responsible for guaranteeing that @p ptr points to
         * at least @p N readable elements; the constructor performs no
         * bounds checking.
         * @param ptr Pointer to a contiguous buffer of length @p N.
         */
        constexpr explicit tvec(const T *ptr) noexcept {
            for (int i = 0; i < N; ++i) {
                values[i] = ptr[i];
            }
        }

        /**
         * @brief Mutable pointer to the first component for raw-buffer access.
         * Returned pointer is valid as long as the @c tvec instance is alive
         * and indexes @c [0, N).
         * @return Pointer to the underlying contiguous storage.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return values;
        }

        /**
         * @brief Const pointer to the first component for raw-buffer access.
         * @return Const pointer to the underlying contiguous storage.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return values;
        }

        /**
         * @brief Component access by index, no bounds checking.
         * @param i Component index in @c [0, N).
         * @return Mutable reference to the @p i-th component.
         */
        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return values[i];
        }

        /**
         * @brief Const component access by index, no bounds checking.
         * @param i Component index in @c [0, N).
         * @return Const reference to the @p i-th component.
         */
        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return values[i];
        }
    };

    /**
     * @brief 2-component vector specialization with @c .x / @c .y named members.
     * Memory layout is two contiguous @p T scalars, matching @c glm::vec<2, T>.
     * The unnamed-union with the uppercase @c .X / @c .Y aliases is kept for
     * source compatibility with the pre-refactor API and is deprecated.
     */
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

        /**
         * @brief Default-construct to the zero vector @c (0, 0).
         */
        constexpr tvec() noexcept : x(T{}), y(T{}) {
        }

        /**
         * @brief Splat construct: every component receives @p s.
         * @param s Scalar broadcast into both components.
         */
        constexpr explicit tvec(T s) noexcept : x(s), y(s) {
        }

        /**
         * @brief Component-wise construct.
         * @param xv Initial value of the @c x component.
         * @param yv Initial value of the @c y component.
         */
        constexpr tvec(T xv, T yv) noexcept : x(xv), y(yv) {
        }

        /**
         * @brief Construct from a buffer of two contiguous scalars.
         * @param ptr Pointer to at least two readable @p T values.
         */
        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]) {
        }

        /**
         * @brief Implicit conversion from a vector of a different scalar type.
         * Each component is @c static_cast to @p T, matching the conversion
         * operator the old @c TVector type provided. Implicit (non-explicit)
         * because losing this conversion would break a large amount of legacy
         * call-site code.
         * @param other Source vector with scalar type @p U.
         */
        template <typename U> constexpr tvec(const tvec<U, 2> &other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {
        }

        /**
         * @brief Mutable pointer to the first component (@c x).
         * @return Pointer that walks @c x then @c y in memory order.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        /**
         * @brief Const pointer to the first component (@c x).
         * @return Const pointer that walks @c x then @c y in memory order.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        /**
         * @brief Component access by index (0 = x, 1 = y).
         * @param i Component index in @c [0, 2).
         * @return Mutable reference to the @p i-th component.
         */
        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        /**
         * @brief Const component access by index (0 = x, 1 = y).
         * @param i Component index in @c [0, 2).
         * @return Const reference to the @p i-th component.
         */
        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    /**
     * @brief 3-component vector specialization with @c .x / @c .y / @c .z named members.
     * Memory layout is three contiguous @p T scalars, matching @c glm::vec<3, T>.
     * Includes swizzle constructors that promote a @ref tvec<T,2> by appending
     * or prepending a scalar — convenient for the @c (xy, z) /
     * @c (x, yz) idioms common in graphics code.
     */
    template <typename T> struct tvec<T, 3> {
        union {
            struct {
                T x, y, z;
            };
            struct {
                T X, Y, Z;
            };
        };

        /**
         * @brief Default-construct to @c (0, 0, 0).
         */
        constexpr tvec() noexcept : x(T{}), y(T{}), z(T{}) {
        }

        /**
         * @brief Splat construct: every component receives @p s.
         * @param s Scalar broadcast into all three components.
         */
        constexpr explicit tvec(T s) noexcept : x(s), y(s), z(s) {
        }

        /**
         * @brief Component-wise construct.
         * @param xv Initial value of @c x.
         * @param yv Initial value of @c y.
         * @param zv Initial value of @c z.
         */
        constexpr tvec(T xv, T yv, T zv) noexcept : x(xv), y(yv), z(zv) {
        }

        /**
         * @brief Promote a 2-component vector by appending a third scalar.
         * Forms @c (v.x, v.y, zv); equivalent to glm's @c vec3(vec2, float).
         * @param v Source 2-component vector.
         * @param zv Third component to append.
         */
        constexpr tvec(const tvec<T, 2> &v, T zv) noexcept : x(v.x), y(v.y), z(zv) {
        }

        /**
         * @brief Prepend a scalar to a 2-component vector.
         * Forms @c (xv, v.x, v.y); equivalent to glm's @c vec3(float, vec2).
         * @param xv First component.
         * @param v Source 2-component vector that supplies @c y and @c z.
         */
        constexpr tvec(T xv, const tvec<T, 2> &v) noexcept : x(xv), y(v.x), z(v.y) {
        }

        /**
         * @brief Construct from a buffer of three contiguous scalars.
         * @param ptr Pointer to at least three readable @p T values.
         */
        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]) {
        }

        /**
         * @brief Implicit conversion from a 3-vector of a different scalar type.
         * Each component is @c static_cast to @p T. Implicit because too much
         * legacy code would break otherwise.
         * @param other Source vector with scalar type @p U.
         */
        template <typename U> constexpr tvec(const tvec<U, 3> &other) noexcept : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) {
        }

        /**
         * @brief Mutable pointer to the first component (@c x).
         * @return Pointer that walks @c x, @c y, @c z in memory order.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return &x;
        }

        /**
         * @brief Const pointer to the first component (@c x).
         * @return Const pointer that walks @c x, @c y, @c z in memory order.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return &x;
        }

        /**
         * @brief Component access by index (0 = x, 1 = y, 2 = z).
         * @param i Component index in @c [0, 3).
         * @return Mutable reference to the @p i-th component.
         */
        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        /**
         * @brief Const component access by index (0 = x, 1 = y, 2 = z).
         * @param i Component index in @c [0, 3).
         * @return Const reference to the @p i-th component.
         */
        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    /**
     * @brief 4-component vector specialization with @c .x / @c .y / @c .z / @c .w named members.
     * Memory layout is four contiguous @p T scalars, matching @c glm::vec<4, T>.
     * Includes the full set of swizzle constructors that combine 2- and
     * 3-component vectors with loose scalars in any sensible order — these
     * mirror glm's overload set so the same idioms work in both libraries.
     */
    template <typename T> struct tvec<T, 4> {
        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T X, Y, Z, W;
            };
        };

        /**
         * @brief Default-construct to @c (0, 0, 0, 0).
         */
        constexpr tvec() noexcept : x(T{}), y(T{}), z(T{}), w(T{}) {
        }

        /**
         * @brief Splat construct: every component receives @p s.
         * @param s Scalar broadcast into all four components.
         */
        constexpr explicit tvec(T s) noexcept : x(s), y(s), z(s), w(s) {
        }

        /**
         * @brief Component-wise construct.
         * @param xv Initial value of @c x.
         * @param yv Initial value of @c y.
         * @param zv Initial value of @c z.
         * @param wv Initial value of @c w.
         */
        constexpr tvec(T xv, T yv, T zv, T wv) noexcept : x(xv), y(yv), z(zv), w(wv) {
        }

        /**
         * @brief Promote a 3-vector by appending a fourth scalar.
         * Forms @c (v.x, v.y, v.z, wv); equivalent to glm's @c vec4(vec3, float).
         * @param v Source 3-component vector.
         * @param wv Fourth component to append.
         */
        constexpr tvec(const tvec<T, 3> &v, T wv) noexcept : x(v.x), y(v.y), z(v.z), w(wv) {
        }

        /**
         * @brief Prepend a scalar to a 3-vector.
         * Forms @c (xv, v.x, v.y, v.z); equivalent to glm's @c vec4(float, vec3).
         * @param xv First component.
         * @param v Source 3-component vector that supplies @c y, @c z, @c w.
         */
        constexpr tvec(T xv, const tvec<T, 3> &v) noexcept : x(xv), y(v.x), z(v.y), w(v.z) {
        }

        /**
         * @brief Append two scalars to a 2-vector.
         * Forms @c (v.x, v.y, zv, wv); equivalent to glm's @c vec4(vec2, float, float).
         * @param v Source 2-component vector that supplies @c x and @c y.
         * @param zv Third component.
         * @param wv Fourth component.
         */
        constexpr tvec(const tvec<T, 2> &v, T zv, T wv) noexcept : x(v.x), y(v.y), z(zv), w(wv) {
        }

        /**
         * @brief Insert a 2-vector between two scalars.
         * Forms @c (xv, v.x, v.y, wv).
         * @param xv First component.
         * @param v Source 2-component vector that supplies @c y and @c z.
         * @param wv Fourth component.
         */
        constexpr tvec(T xv, const tvec<T, 2> &v, T wv) noexcept : x(xv), y(v.x), z(v.y), w(wv) {
        }

        /**
         * @brief Append a 2-vector to two scalars.
         * Forms @c (xv, yv, v.x, v.y).
         * @param xv First component.
         * @param yv Second component.
         * @param v Source 2-component vector that supplies @c z and @c w.
         */
        constexpr tvec(T xv, T yv, const tvec<T, 2> &v) noexcept : x(xv), y(yv), z(v.x), w(v.y) {
        }

        /**
         * @brief Concatenate two 2-vectors.
         * Forms @c (a.x, a.y, b.x, b.y); equivalent to glm's @c vec4(vec2, vec2).
         * @param a Source vector that supplies @c x and @c y.
         * @param b Source vector that supplies @c z and @c w.
         */
        constexpr tvec(const tvec<T, 2> &a, const tvec<T, 2> &b) noexcept : x(a.x), y(a.y), z(b.x), w(b.y) {
        }

        /**
         * @brief Construct from a buffer of four contiguous scalars.
         * @param ptr Pointer to at least four readable @p T values.
         */
        constexpr explicit tvec(const T *ptr) noexcept : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {
        }

        /**
         * @brief Implicit conversion from a 4-vector of a different scalar type.
         * Each component is @c static_cast to @p T.
         * @param other Source vector with scalar type @p U.
         */
        template <typename U>
        constexpr tvec(const tvec<U, 4> &other) noexcept : x(static_cast<T>(other.x)),
                                                           y(static_cast<T>(other.y)),
                                                           z(static_cast<T>(other.z)),
                                                           w(static_cast<T>(other.w)) {
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
         * @brief Component access by index (0 = x, 1 = y, 2 = z, 3 = w).
         * @param i Component index in @c [0, 4).
         * @return Mutable reference to the @p i-th component.
         */
        [[nodiscard]] constexpr T &operator[](int i) noexcept {
            return (&x)[i];
        }

        /**
         * @brief Const component access by index (0 = x, 1 = y, 2 = z, 3 = w).
         * @param i Component index in @c [0, 4).
         * @return Const reference to the @p i-th component.
         */
        [[nodiscard]] constexpr const T &operator[](int i) const noexcept {
            return (&x)[i];
        }
    };

    // ---------------------------------------------------------------------
    // Unary & binary arithmetic operators (componentwise).
    // ---------------------------------------------------------------------

    /**
     * @brief Unary plus — returns a copy of @p v unchanged.
     * Provided for symmetry with @ref operator-(const tvec<T,N>&) so generic
     * code can apply a sign without dispatching on @p N.
     * @param v Vector to copy.
     * @return A copy of @p v.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator+(const tvec<T, N> &v) noexcept {
        return v;
    }

    /**
     * @brief Componentwise negation.
     * @param v Vector to negate.
     * @return A vector whose components are the negation of @p v's components.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator-(const tvec<T, N> &v) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = -v[i];
        }
        return r;
    }

    /**
     * @brief Componentwise addition.
     * @param a Left operand.
     * @param b Right operand.
     * @return The componentwise sum @c a[i] + b[i].
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator+(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] + b[i];
        }
        return r;
    }

    /**
     * @brief Componentwise subtraction.
     * @param a Left operand.
     * @param b Right operand.
     * @return The componentwise difference @c a[i] - b[i].
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator-(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] - b[i];
        }
        return r;
    }

    /**
     * @brief Componentwise (Hadamard) product.
     * Note: this is the per-element product, not a dot product. Use
     * @ref dot for the inner product or @ref cross for the cross product.
     * @param a Left operand.
     * @param b Right operand.
     * @return The componentwise product @c a[i] * b[i].
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] * b[i];
        }
        return r;
    }

    /**
     * @brief Componentwise division.
     * Each component of @p a is divided by the matching component of @p b;
     * dividing by zero is undefined behaviour for the underlying scalar type.
     * @param a Numerator vector.
     * @param b Denominator vector.
     * @return The componentwise quotient @c a[i] / b[i].
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator/(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] / b[i];
        }
        return r;
    }

    /**
     * @brief Vector-by-scalar multiplication.
     * @param v Vector operand.
     * @param s Scalar operand.
     * @return The vector whose components are @c v[i] * s.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const tvec<T, N> &v, const T s) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = v[i] * s;
        }
        return r;
    }

    /**
     * @brief Scalar-by-vector multiplication (commutative form).
     * Forwards to @ref operator*(const tvec<T,N>&, T) so the two argument
     * orders behave identically.
     * @param s Scalar operand.
     * @param v Vector operand.
     * @return The vector whose components are @c v[i] * s.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator*(const T s, const tvec<T, N> &v) noexcept {
        return v * s;
    }

    /**
     * @brief Vector-by-scalar division.
     * @param v Vector operand.
     * @param s Scalar divisor; behaviour is undefined when zero.
     * @return The vector whose components are @c v[i] / s.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> operator/(const tvec<T, N> &v, const T s) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = v[i] / s;
        }
        return r;
    }

    // Compound assignments.

    /**
     * @brief In-place componentwise addition.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator+=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] += b[i];
        }
        return a;
    }

    /**
     * @brief In-place componentwise subtraction.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator-=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] -= b[i];
        }
        return a;
    }

    /**
     * @brief In-place componentwise (Hadamard) multiplication.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator*=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] *= b[i];
        }
        return a;
    }

    /**
     * @brief In-place componentwise division.
     * @param a Numerator vector, modified in place.
     * @param b Denominator vector.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator/=(tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] /= b[i];
        }
        return a;
    }

    /**
     * @brief In-place vector-by-scalar multiplication.
     * @param a Vector operand, modified in place.
     * @param s Scalar multiplier.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator*=(tvec<T, N> &a, const T s) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] *= s;
        }
        return a;
    }

    /**
     * @brief In-place vector-by-scalar division.
     * @param a Vector operand, modified in place.
     * @param s Scalar divisor; behaviour is undefined when zero.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tvec<T, N> &operator/=(tvec<T, N> &a, const T s) noexcept {
        for (int i = 0; i < N; ++i) {
            a[i] /= s;
        }
        return a;
    }

    // Equality.

    /**
     * @brief Exact equality comparison.
     * Performs a strict per-component comparison; for floating-point scalars
     * prefer @ref equals so rounding noise does not produce false negatives.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when every component of @p a equals the matching component of @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr bool operator==(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        for (int i = 0; i < N; ++i) {
            if (!(a[i] == b[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Exact inequality comparison.
     * Negation of @ref operator==(const tvec<T,N>&, const tvec<T,N>&).
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when at least one component of @p a differs from @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr bool operator!=(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return !(a == b);
    }

    // ---------------------------------------------------------------------
    // Geometric functions (glm-compatible names).
    // ---------------------------------------------------------------------

    /**
     * @brief Inner (dot) product of two vectors.
     * Equivalent to @c glm::dot. Uses an explicit accumulation loop so the
     * result is @c constexpr-friendly for any @p N.
     * @param a Left operand.
     * @param b Right operand.
     * @return The sum @c a[0]*b[0] + a[1]*b[1] + ... + a[N-1]*b[N-1].
     */
    template <typename T, int N> [[nodiscard]] constexpr T dot(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        T acc = T{};
        for (int i = 0; i < N; ++i) {
            acc += a[i] * b[i];
        }
        return acc;
    }

    /**
     * @brief 3D cross product.
     * Equivalent to @c glm::cross. Right-handed convention: @c cross(x, y) = z.
     * @param a Left operand.
     * @param b Right operand.
     * @return The vector perpendicular to both inputs whose magnitude is the area of the parallelogram they span.
     */
    template <typename T> [[nodiscard]] constexpr tvec<T, 3> cross(const tvec<T, 3> &a, const tvec<T, 3> &b) noexcept {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
    }

    /**
     * @brief 2D scalar (perp-dot) cross product.
     * Returns a single scalar — the @c z-component of the equivalent 3D cross
     * product. Sign indicates orientation: positive when @p b is
     * counter-clockwise of @p a.
     * @param a Left operand.
     * @param b Right operand.
     * @return The scalar @c a.x * b.y - a.y * b.x.
     */
    template <typename T> [[nodiscard]] constexpr T cross(const tvec<T, 2> &a, const tvec<T, 2> &b) noexcept {
        return a.x * b.y - a.y * b.x;
    }

    /**
     * @brief Squared Euclidean length, @c glm::length2 equivalent.
     * Cheaper than @ref length because it skips the square root, which makes
     * it the right choice for length comparisons (sort by distance, sphere
     * containment tests, etc.).
     * @param v Input vector.
     * @return The dot product of @p v with itself.
     */
    template <typename T, int N> [[nodiscard]] constexpr T length2(const tvec<T, N> &v) noexcept {
        return dot(v, v);
    }

    /**
     * @brief Euclidean length (L2 norm), @c glm::length equivalent.
     * Calls @c std::sqrt and is therefore not @c constexpr in C++17. Prefer
     * @ref length2 when only ordering matters.
     * @param v Input vector.
     * @return @c sqrt(length2(v)).
     */
    template <typename T, int N> [[nodiscard]] T length(const tvec<T, N> &v) {
        return static_cast<T>(std::sqrt(length2(v)));
    }

    /**
     * @brief Return the unit-length version of @p v.
     * Behaviour is undefined when @p v is the zero vector (division by zero).
     * Equivalent to @c glm::normalize.
     * @param v Input vector; should be non-zero.
     * @return A vector parallel to @p v with length 1.
     */
    template <typename T, int N> [[nodiscard]] tvec<T, N> normalize(const tvec<T, N> &v) {
        return v / length(v);
    }

    /**
     * @brief Componentwise minimum of two vectors.
     * Equivalent to @c glm::min(vec, vec). Useful for AABB shrinking.
     * @param a Left operand.
     * @param b Right operand.
     * @return A vector whose components are the per-component minimum of @p a and @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> min(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] < b[i] ? a[i] : b[i];
        }
        return r;
    }

    /**
     * @brief Componentwise maximum of two vectors.
     * Equivalent to @c glm::max(vec, vec). Useful for AABB growing.
     * @param a Left operand.
     * @param b Right operand.
     * @return A vector whose components are the per-component maximum of @p a and @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> max(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        tvec<T, N> r;
        for (int i = 0; i < N; ++i) {
            r[i] = a[i] > b[i] ? a[i] : b[i];
        }
        return r;
    }

    /**
     * @brief Stream insertion for debugging and test failure reports.
     * Writes the vector in the form @c "xe::tvec<N>{ a, b, c }". Not
     * intended as a serialisation format.
     * @param os Output stream.
     * @param v Vector to print.
     * @return The same stream, to allow chaining.
     */
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

    using vec2 = tvec<float, 2>; ///< 2-component @c float vector — glm-compatible alias.
    using vec3 = tvec<float, 3>; ///< 3-component @c float vector — glm-compatible alias.
    using vec4 = tvec<float, 4>; ///< 4-component @c float vector — glm-compatible alias.

    using ivec2 = tvec<std::int32_t, 2>; ///< 2-component signed 32-bit integer vector.
    using ivec3 = tvec<std::int32_t, 3>; ///< 3-component signed 32-bit integer vector.
    using ivec4 = tvec<std::int32_t, 4>; ///< 4-component signed 32-bit integer vector.

    using dvec2 = tvec<double, 2>; ///< 2-component @c double vector.
    using dvec3 = tvec<double, 3>; ///< 3-component @c double vector.
    using dvec4 = tvec<double, 4>; ///< 4-component @c double vector.

    using uvec2 = tvec<std::uint32_t, 2>; ///< 2-component unsigned 32-bit integer vector.
    using uvec3 = tvec<std::uint32_t, 3>; ///< 3-component unsigned 32-bit integer vector.
    using uvec4 = tvec<std::uint32_t, 4>; ///< 4-component unsigned 32-bit integer vector.

    // ---------------------------------------------------------------------
    // Legacy PascalCase aliases (kept here so any header that only pulls in
    // Vector.h still sees the old names). See Legacy.h for the full set of
    // deprecated forwarders.
    // ---------------------------------------------------------------------

    template <typename T, int N> using TVector = tvec<T, N>; ///< @deprecated Use @ref tvec.
    template <typename T> using TVector2 = tvec<T, 2>;       ///< @deprecated Use @ref tvec<T,2>.
    template <typename T> using TVector3 = tvec<T, 3>;       ///< @deprecated Use @ref tvec<T,3>.
    template <typename T> using TVector4 = tvec<T, 4>;       ///< @deprecated Use @ref tvec<T,4>.

    using Vector2 = vec2; ///< @deprecated Use @ref vec2.
    using Vector3 = vec3; ///< @deprecated Use @ref vec3.
    using Vector4 = vec4; ///< @deprecated Use @ref vec4.

    using Vector2i = ivec2; ///< @deprecated Use @ref ivec2.
    using Vector3i = ivec3; ///< @deprecated Use @ref ivec3.
    using Vector4i = ivec4; ///< @deprecated Use @ref ivec4.

    using Vector2d = dvec2; ///< @deprecated Use @ref dvec2.
    using Vector3d = dvec3; ///< @deprecated Use @ref dvec3.
    using Vector4d = dvec4; ///< @deprecated Use @ref dvec4.

    /**
     * @brief Legacy alias for @ref length.
     * @deprecated Use @ref length(const tvec<T,N>&) instead.
     * @see length
     * @param v Input vector.
     * @return Euclidean length of @p v.
     */
    template <typename T, int N> [[nodiscard]] T norm(const tvec<T, N> &v) {
        return length(v);
    }

    /**
     * @brief Legacy alias for @ref length2.
     * @deprecated Use @ref length2(const tvec<T,N>&) instead.
     * @see length2
     * @param v Input vector.
     * @return Squared length of @p v.
     */
    template <typename T, int N> [[nodiscard]] constexpr T norm2(const tvec<T, N> &v) noexcept {
        return length2(v);
    }

    /**
     * @brief Legacy alias for @ref max.
     * @deprecated Use @ref max(const tvec<T,N>&, const tvec<T,N>&) instead.
     * @see max
     * @param a Left operand.
     * @param b Right operand.
     * @return Componentwise maximum of @p a and @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> maximize(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return max(a, b);
    }

    /**
     * @brief Legacy alias for @ref min.
     * @deprecated Use @ref min(const tvec<T,N>&, const tvec<T,N>&) instead.
     * @see min
     * @param a Left operand.
     * @param b Right operand.
     * @return Componentwise minimum of @p a and @p b.
     */
    template <typename T, int N> [[nodiscard]] constexpr tvec<T, N> minimize(const tvec<T, N> &a, const tvec<T, N> &b) noexcept {
        return min(a, b);
    }
} // namespace xe

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
