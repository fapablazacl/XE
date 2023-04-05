
#pragma once

#ifndef __XE_MATH_VECTOR_HPP__
#define __XE_MATH_VECTOR_HPP__

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // non-standard extension used: nameless struct/union
#endif

namespace XE {
    template <typename T, int N> struct VectorBase {
        T values[N];
    };

    template <typename T> struct VectorBase<T, 2> {
        union {
            T values[2];
            struct {
                T X, Y;
            };
        };

        VectorBase() { X = Y = static_cast<T>(0); }

        VectorBase(T x, T y) {
            X = x;
            Y = y;
        }
    };

    template <typename T> struct VectorBase<T, 3> {
        union {
            T values[3];
            struct {
                T X, Y, Z;
            };
        };

        VectorBase() { X = Y = Z = static_cast<T>(0); }

        VectorBase(T x, T y, T z) {
            X = x;
            Y = y;
            Z = z;
        }

        VectorBase(const VectorBase<T, 2> &v, T z) {
            X = v.X;
            Y = v.Y;
            Z = z;
        }

        VectorBase(T x, const VectorBase<T, 2> &v) {
            X = x;
            Y = v.X;
            Z = v.Y;
        }
    };

    template <typename T> struct VectorBase<T, 4> {
        union {
            T values[4];
            struct {
                T X, Y, Z, W;
            };
        };

        VectorBase() { X = Y = Z = W = static_cast<T>(0); }

        VectorBase(T x, T y, T z, T w) {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }

        VectorBase(const VectorBase<T, 3> &v, T w) {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
            W = w;
        }

        VectorBase(T x, const VectorBase<T, 3> &v) {
            X = x;
            Y = v.X;
            Z = v.T;
            W = v.W;
        }

        VectorBase(const VectorBase<T, 2> &v, T z, T w) {
            X = v.X;
            Y = v.Y;
            Z = z;
            W = w;
        }

        VectorBase(T x, const VectorBase<T, 2> &v, T w) {
            X = x;
            Y = v.X;
            Z = v.Y;
            W = w;
        }

        VectorBase(T x, T y, const VectorBase<T, 2> &v) {
            X = x;
            Y = y;
            Z = v.X;
            W = v.Y;
        }
    };

    /**
     * @brief N-Dimensional Vector structure.
     */
    template <typename T, int N> struct Vector : public VectorBase<T, N> {
        using VectorBase<T, N>::VectorBase;

        explicit Vector();

        explicit Vector(T value);

        explicit Vector(const T *values);

        Vector<T, N> operator+() const;

        Vector<T, N> operator-() const;

        Vector<T, N> operator+(const Vector<T, N> &rhs) const;

        Vector<T, N> operator-(const Vector<T, N> &rhs) const;

        Vector<T, N> operator*(const Vector<T, N> &rhs) const;

        Vector<T, N> operator/(const Vector<T, N> &rhs) const;

        Vector<T, N> operator*(const T rhs) const;

        Vector<T, N> operator/(const T rhs) const;

        Vector<T, N> &operator+=(const Vector<T, N> &rhs);

        Vector<T, N> &operator-=(const Vector<T, N> &rhs);

        Vector<T, N> &operator*=(const Vector<T, N> &rhs);

        Vector<T, N> &operator/=(const Vector<T, N> &rhs);

        Vector<T, N> &operator*=(const T rhs);

        Vector<T, N> &operator/=(const T rhs);

        bool operator==(const Vector<T, N> &rhs) const;

        bool operator!=(const Vector<T, N> &rhs) const;

        T *data() { return &this->values[0]; }

        const T *data() const { return &this->values[0]; }

        T &operator[](const int index) { return this->values[index]; }

        const T operator[](const int index) const { return this->values[index]; }

        inline friend Vector<T, N> operator*(const T s, const Vector<T, N> &v) { return v * s; }

        template <typename T2> operator Vector<T2, N>() const {
            Vector<T2, N> result;

            for (int i = 0; i < N; i++) {
                result.values[i] = static_cast<T2>(this->values[i]);
            }

            return result;
        }

        template <typename T2> Vector<T2, N> cast() const {
            Vector<T2, N> result;

            for (int i = 0; i < N; i++) {
                result.values[i] = static_cast<T2>(this->values[i]);
            }

            return result;
        }
    };

    template <typename T, int N> T dot(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        T sum = T(0);

        for (int i = 0; i < N; i++) {
            sum += v1.values[i] * v2.values[i];
        }

        return sum;
    }

    template <typename T> Vector<T, 3> cross(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
        return {v1.Y * v2.Z - v1.Z * v2.Y, v1.Z * v2.X - v1.X * v2.Z, v1.X * v2.Y - v1.Y * v2.X};
    }

    template <typename T> T cross(const Vector<T, 2> &v1, const Vector<T, 2> &v2) { return v1.X * v2.Y - v1.Y * v2.X; }

    template <typename T> Vector<T, 3> triple_cross(const Vector<T, 3> &v1, const Vector<T, 3> &v2, const Vector<T, 3> &v3) { return cross(cross(v1, v2), v3); }

    template <typename T> T triple_dot(const Vector<T, 3> &v1, const Vector<T, 3> &v2, const Vector<T, 3> &v3) { return dot(cross(v1, v2), v3); }

    template <typename T, int N> T norm(const Vector<T, N> &v) { return static_cast<T>(std::sqrt(dot(v, v))); }

    template <typename T, int N> T norm2(const Vector<T, N> &v) { return dot(v, v); }

    template <typename T, int N> Vector<T, N> normalize(const Vector<T, N> &v) { return v / norm(v); }

    template <typename T, int N> Vector<T, N> maximize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result[i] = std::max(v1[i], v2[i]);
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> minimize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result[i] = std::min(v1[i], v2[i]);
        }

        return result;
    }

    typedef Vector<std::int32_t, 2> Vector2i;
    typedef Vector<std::int32_t, 3> Vector3i;
    typedef Vector<std::int32_t, 4> Vector4i;

    typedef Vector<float, 2> Vector2f;
    typedef Vector<float, 3> Vector3f;
    typedef Vector<float, 4> Vector4f;

    typedef Vector<double, 2> Vector2d;
    typedef Vector<double, 3> Vector3d;
    typedef Vector<double, 4> Vector4d;

    template <typename T, int N> Vector<T, N>::Vector() {}

    template <typename T, int N> Vector<T, N>::Vector(T value) {
        for (T &element : this->values) {
            element = value;
        }
    }

    template <typename T, int N> Vector<T, N>::Vector(const T *values) {
        assert(values);

        for (int i = 0; i < N; i++) {
            this->values[i] = values[i];
        }
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator+() const { return *this; }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator-() const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = -this->values[i];
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator+(const Vector<T, N> &rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] + rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator-(const Vector<T, N> &rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] - rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator*(const Vector<T, N> &rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] * rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator/(const Vector<T, N> &rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] / rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator*(const T rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] * rhs;
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> Vector<T, N>::operator/(const T rhs) const {
        Vector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] / rhs;
        }

        return result;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator+=(const Vector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] += rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator-=(const Vector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] -= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator*=(const Vector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] *= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator/=(const Vector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] /= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator*=(const T rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] *= rhs;
        }

        return *this;
    }

    template <typename T, int N> Vector<T, N> &Vector<T, N>::operator/=(const T rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] /= rhs;
        }

        return *this;
    }

    template <typename T, int N> bool Vector<T, N>::operator==(const Vector<T, N> &rhs) const {
        for (int i = 0; i < N; i++) {
            if (this->values[i] != rhs.values[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename T, int N> bool Vector<T, N>::operator!=(const Vector<T, N> &rhs) const { return !(*this == rhs); }

    template <typename T, int N> inline std::ostream &operator<<(std::ostream &os, const Vector<T, N> &v) {
        os << "XE::Vector<" << typeid(T).name() << ", " << N << ">{ ";

        for (int i = 0; i < N - 1; i++) {
            os << v[i] << ", ";
        }

        os << v[N - 1] << " }";

        return os;
    }

    template <typename T> using Vector2 = Vector<T, 2>;

    template <typename T> using Vector3 = Vector<T, 3>;

    template <typename T> using Vector4 = Vector<T, 4>;
} // namespace XE

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
