
#ifndef __XE_MATH_QUATERNION_HPP__
#define __XE_MATH_QUATERNION_HPP__

#include <cassert>
#include <cmath>
#include <cstdint>

#include "Common.h"
#include "Rotation.h"
#include "Vector.h"

/*
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201)
#endif
 */

namespace XE {
    template <typename T> struct Quaternion {
        union {
            struct {
                Vector<T, 3> V;
                T W;
            };

            T data[4];
        };

        Quaternion() {}

        explicit Quaternion(const T w) {
            V.X = V.Y = V.Z = T(0);
            W = w;
        }

        explicit Quaternion(const T *values) {
            assert(values);

            for (int i = 0; i < 4; i++) {
                data[i] = values[i];
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
                result.data[i] = this->data[i] + rhs.data[i];
            }

            return result;
        }

        Quaternion<T> operator-(const Quaternion<T> &rhs) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.data[i] = this->data[i] - rhs.data[i];
            }

            return result;
        }

        Quaternion<T> operator-() const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.data[i] = -this->data[i];
            }

            return result;
        }

        Quaternion<T> operator+() const { return *this; }

        Quaternion<T> operator*(const Quaternion<T> &rhs) const { return {cross(V, rhs.V) + rhs.V * W + V * rhs.W, W * rhs.W - dot(V, rhs.V)}; }

        Quaternion<T> operator/(const Quaternion<T> &rhs) const { return (*this) * inverse(rhs); }

        Quaternion<T> operator*(const T s) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.data[i] = this->data[i] * s;
            }

            return result;
        }

        Quaternion<T> operator/(const T s) const {
            Quaternion<T> result;

            for (int i = 0; i < 4; i++) {
                result.data[i] = this->data[i] / s;
            }

            return result;
        }

        friend Quaternion<T> operator*(const T s, const Quaternion<T> &q) { return q * s; }

        Quaternion<T> &operator+=(const Quaternion<T> &rhs) {
            for (int i = 0; i < 4; i++) {
                this->data[i] += rhs.data[i];
            }

            return *this;
        }

        Quaternion<T> &operator-=(const Quaternion<T> &rhs) {
            for (int i = 0; i < 4; i++) {
                this->data[i] -= rhs.data[i];
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
                this->data[i] *= s;
            }

            return *this;
        }

        Quaternion<T> &operator/=(const T s) {
            for (int i = 0; i < 4; i++) {
                this->data[i] /= s;
            }

            return *this;
        }

        bool operator==(const Quaternion<T> &rhs) const {
            for (int i = 0; i < 4; i++) {
                if (! equals(data[i], rhs.data[i])) {
                    return false;
                }
            }

            return true;
        }

        bool operator!=(const Quaternion<T> &rhs) const { return !(*this == rhs); }

        static Quaternion<T> createZero() { return Quaternion<T>({T(0), T(0), T(0)}, T(0)); }

        static Quaternion<T> createIdentity() { return Quaternion<T>({T(0), T(0), T(0)}, T(1)); }

        static Quaternion<T> createRotation(const Vector<T, 3> &axis, const T radians) {
            auto q = Quaternion<T>(axis, std::cos(radians / T(2)));

            return normalize(q);
        }

        static Quaternion<T> createRotation(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
            auto v = cross(v1, v2);
            auto w = std::sqrt(dot(v1, v1) * dot(v2, v2)) + dot(v1, v2);

            return normalize(Quaternion<T>(v, w));
        }
    };

    template <typename T> Quaternion<T> conjugate(const Quaternion<T> &q) { return {-q.V, q.W}; }

    template <typename T> Quaternion<T> inverse(const Quaternion<T> &q) { return conjugate(q) / norm2(q); }

    template <typename T> T dot(const Quaternion<T> &q1, const Quaternion<T> &q2) {
        T sum = T(0);

        for (int i = 0; i < 4; i++) {
            sum += q1.data[i] * q2.data[i];
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
} // namespace XE

/*
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
*/

#endif
