
#ifndef __XE_MATH_QUATERNION_HPP__
#define __XE_MATH_QUATERNION_HPP__

#include <cassert>
#include <cmath>
#include <cstdint>

#include "Common.h"
#include "Rotation.h"
#include "Vector.h"


#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // non-standard extension used: nameless struct/union
#endif


namespace XE {
    template <typename T> struct Quaternion {
        union {
            struct {
                Vector<T, 3> V;
                T W;
            };

            T values[4];
        };

        Quaternion() {
            V.X = V.Y = V.Z = W = T(0);
        }

        explicit Quaternion(const T w) {
            V.X = V.Y = V.Z = T(0);
            W = w;
        }

        explicit Quaternion(const T *values) {
            assert(values);

            for (int i = 0; i < 4; i++) {
                this->values[i] = values[i];
            }
        }

        Quaternion(const T x, const T y, const T z, const T w) {
            V.X = x;
            V.Y = y;
            V.Z = z;
            W = w;
        }

        explicit Quaternion(const Vector<T, 3> &v) {
            V = v;
            W = T(0);
        }

        explicit Quaternion(const T x, const T y, const T z) {
            V.X = x;
            V.Y = y;
            V.Z = z;
            W = T(0);
        }

        Quaternion(const Vector<T, 3> &v, T w) {
            V = v;
            W = w;
        }

        explicit Quaternion(const Vector<T, 4> &v) {
            V.X = v.X;
            V.Y = v.Y;
            V.Z = v.Z;
            W = v.W;
        }

        Quaternion(const Quaternion<T> &other) {
            V = other.V;
            W = other.W;
        }

        constexpr size_t size() const {
            return 4;
        }

        T* data() {
            return &values[0];
        }

        const T* data() const {
            return &values[0];
        }

        constexpr const T& operator[](const size_t index) const {
            assert(index < 4);

            return values[index];
        }

        constexpr T& operator[](const size_t index) {
            assert(index < 4);

            return values[index];
        }

        explicit operator Rotation<T>() const {
            const T angle = T(2) * std::acos(W);

            if (angle == T(0)) {
                return {angle, {T(1), T(0), T(0)}};
            } else {
                return {angle, normalize(V)};
            }
        }

        explicit operator Vector<T, 4>() const { return {V.X, V.Y, V.Z, V.W}; }

        Quaternion<T> operator+(const Quaternion<T> &rhs) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.values[i] = this->values[i] + rhs.values[i];
            }

            return result;
        }

        Quaternion<T> operator-(const Quaternion<T> &rhs) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.values[i] = this->values[i] - rhs.values[i];
            }

            return result;
        }

        Quaternion<T> operator-() const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.values[i] = -this->values[i];
            }

            return result;
        }

        Quaternion<T> operator+() const { return *this; }

        Quaternion<T> operator*(const Quaternion<T> &rhs) const { return {cross(V, rhs.V) + rhs.V * W + V * rhs.W, W * rhs.W - dot(V, rhs.V)}; }

        Quaternion<T> operator/(const Quaternion<T> &rhs) const { return (*this) * inverse(rhs); }

        Quaternion<T> operator*(const T s) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.values[i] = this->values[i] * s;
            }

            return result;
        }

        Quaternion<T> operator/(const T s) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.values[i] = this->values[i] / s;
            }

            return result;
        }

        friend Quaternion<T> operator*(const T s, const Quaternion<T> &q) { return q * s; }

        Quaternion<T> &operator+=(const Quaternion<T> &rhs) {
            for (int i = 0; i < 4; i++) {
                this->values[i] += rhs.values[i];
            }

            return *this;
        }

        Quaternion<T> &operator-=(const Quaternion<T> &rhs) {
            for (int i = 0; i < 4; i++) {
                this->values[i] -= rhs.values[i];
            }

            return *this;
        }

        Quaternion<T> &operator*=(const Quaternion<T> &rhs) {
            *this = *this * rhs;

            return *this;
        }

        Quaternion<T> &operator/=(const Quaternion<T> &rhs) {
            *this = *this / rhs;

            return *this;
        }

        Quaternion<T> &operator*=(const T s) {
            for (int i = 0; i < 4; i++) {
                this->values[i] *= s;
            }

            return *this;
        }

        Quaternion<T> &operator/=(const T s) {
            for (int i = 0; i < 4; i++) {
                this->values[i] /= s;
            }

            return *this;
        }

        bool operator==(const Quaternion<T> &rhs) const {
            for (int i = 0; i < 4; i++) {
                if (! equals(values[i], rhs.values[i])) {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const Quaternion<T> &rhs) const { return !(*this == rhs); }
    };

    template <typename T> Quaternion<T> conjugate(const Quaternion<T> &q) { return {-q.V, q.W}; }

    template <typename T> Quaternion<T> inverse(const Quaternion<T> &q) { return conjugate(q) / norm2(q); }

    template <typename T> T dot(const Quaternion<T> &q1, const Quaternion<T> &q2) {
        T sum = T(0);

        for (int i = 0; i < 4; i++) {
            sum += q1.values[i] * q2.values[i];
        }

        return sum;
    }

    template <typename T> T norm2(const Quaternion<T> &q) { return dot(q, q); }

    /**
     * @brief Compute the magnitude, module or length (AKA Absolute Value) for a given Quaternion.
     */
    template <typename T> T norm(const Quaternion<T> &q) { return static_cast<T>(std::sqrt(norm2(q))); }

    /**
     * @brief Compute a Quaternion with a unit magnitude (1)
     */
    template <typename T> Quaternion<T> normalize(const Quaternion<T> &q) { return q / norm(q); }

    template <typename T> Vector<T, 3> transform(const Quaternion<T> &q, const Vector<T, 3> &v) { return (q * Quaternion<T>(v) * inverse(q)).V; }

    template<typename T>
    Quaternion<T> quatId() {
        return Quaternion<T>({T(0), T(0), T(0)}, T(1));
    }

    template<typename T>
    Quaternion<T> quatZero() {
        return Quaternion<T>({T(0), T(0), T(0)}, T(0));
    }

    template<typename T>
    Quaternion<T> quatRotationRH(const Vector<T, 3> &axis, const T radians) {
        assert(equals(norm(axis), T{1}) && "Axis should be normalized");

        const T angle = T{0.5} * radians;

        return Quaternion<T>(std::sin(angle) * axis, std::cos(angle));
    }

    template<typename T>
    Quaternion<T> quatRotationLH(const Vector<T, 3> &axis, const T radians) {
        assert(equals(norm(axis), T{1}) && "Axis should be normalized");

        const T angle = T{0.5} * radians;

        return Quaternion<T>(-std::sin(angle) * axis, std::cos(angle));
    }

    template<typename T>
    Quaternion<T> quatRotationRH(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
        auto v = cross(v1, v2);
        auto w = std::sqrt(dot(v1, v1) * dot(v2, v2)) + dot(v1, v2);

        return normalize(Quaternion<T>(v, w));
    }
} // namespace XE

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
