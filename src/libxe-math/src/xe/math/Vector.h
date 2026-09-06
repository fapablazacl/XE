
#pragma once

#ifndef __XE_MATH_VECTOR_HPP__
#define __XE_MATH_VECTOR_HPP__

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201) // non-standard extension used: nameless struct/union
#endif

namespace xe {
    template <typename T, int N> struct VectorBase {
        T values[N] = {};

        constexpr VectorBase() {
        }

        constexpr VectorBase(std::initializer_list<T> il) {
            assert(il.size() == N);

            int i = 0;
            for (const T value : il) {
                values[i++] = value;
            }
        }
    };

    template <typename T> struct VectorBase<T, 2> {
        union {
            T values[2];
            struct {
                T x, y;
            };
        };

        constexpr VectorBase() {
            x = y = static_cast<T>(0);
        }

        constexpr VectorBase(T const x, T const y) : x(x), y(y) {}
    };

    template <typename T> struct VectorBase<T, 3> {
        union {
            T values[3];
            struct {
                T x, y, z;
            };
        };

        constexpr VectorBase() {
            x = y = z = static_cast<T>(0);
        }

        constexpr VectorBase(T const x, T const y, T const z) : x(x), y(y), z(z) {}
    };

    template <typename T> struct VectorBase<T, 4> {
        union {
            T values[4];
            struct {
                T x, y, z, w;
            };
        };

        constexpr VectorBase() {
            x = y = z = w = static_cast<T>(0);
        }

        constexpr VectorBase(T const x, T const y, T const z, T const w) : x(x), y(y), z(z), w(w) {}
    };

    /**
     * @brief N-Dimensional TVector structure.
     */
    template <typename T, int N> struct TVector : public VectorBase<T, N> {
        using VectorBase<T, N>::VectorBase;

        TVector();

        explicit TVector(T value);

        explicit TVector(const T *values);

        TVector<T, N> operator+() const;

        TVector<T, N> operator-() const;

        TVector<T, N> operator+(const TVector<T, N> &rhs) const;

        TVector<T, N> operator-(const TVector<T, N> &rhs) const;

        TVector<T, N> operator*(const TVector<T, N> &rhs) const;

        TVector<T, N> operator/(const TVector<T, N> &rhs) const;

        TVector<T, N> operator*(const T rhs) const;

        TVector<T, N> operator/(const T rhs) const;

        TVector<T, N> &operator+=(const TVector<T, N> &rhs);

        TVector<T, N> &operator-=(const TVector<T, N> &rhs);

        TVector<T, N> &operator*=(const TVector<T, N> &rhs);

        TVector<T, N> &operator/=(const TVector<T, N> &rhs);

        TVector<T, N> &operator*=(const T rhs);

        TVector<T, N> &operator/=(const T rhs);

        bool operator==(const TVector<T, N> &rhs) const;

        bool operator!=(const TVector<T, N> &rhs) const;

        T *data() {
            return &this->values[0];
        }

        const T *data() const {
            return &this->values[0];
        }

        T &operator[](const int index) {
            return this->values[index];
        }

        const T operator[](const int index) const {
            return this->values[index];
        }

        inline friend TVector<T, N> operator*(const T s, const TVector<T, N> &v) {
            return v * s;
        }

        template <typename T2> operator TVector<T2, N>() const {
            TVector<T2, N> result;

            for (int i = 0; i < N; i++) {
                result.values[i] = static_cast<T2>(this->values[i]);
            }

            return result;
        }

        template <typename T2> TVector<T2, N> cast() const {
            TVector<T2, N> result;

            for (int i = 0; i < N; i++) {
                result.values[i] = static_cast<T2>(this->values[i]);
            }

            return result;
        }
    };

    template <typename T, int N> T dot(const TVector<T, N> &v1, const TVector<T, N> &v2) {
        T sum = T(0);

        for (int i = 0; i < N; i++) {
            sum += v1.values[i] * v2.values[i];
        }

        return sum;
    }

    template <typename T> TVector<T, 3> cross(const TVector<T, 3> &v1, const TVector<T, 3> &v2) {
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    }

    template <typename T> T cross(const TVector<T, 2> &v1, const TVector<T, 2> &v2) {
        return v1.x * v2.y - v1.y * v2.x;
    }

    template <typename T> TVector<T, 3> cross(const TVector<T, 3> &v1, const TVector<T, 3> &v2, const TVector<T, 3> &v3) {
        return cross(cross(v1, v2), v3);
    }

    template <typename T> T dot(const TVector<T, 3> &v1, const TVector<T, 3> &v2, const TVector<T, 3> &v3) {
        return dot(cross(v1, v2), v3);
    }

    template <typename T, int N> T norm(const TVector<T, N> &v) {
        return static_cast<T>(std::sqrt(dot(v, v)));
    }

    template <typename T, int N> T norm2(const TVector<T, N> &v) {
        return dot(v, v);
    }

    template <typename T, int N> TVector<T, N> normalize(const TVector<T, N> &v) {
        return v / norm(v);
    }

    template <typename T, int N> TVector<T, N> maximize(const TVector<T, N> &v1, const TVector<T, N> &v2) {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result[i] = std::max(v1[i], v2[i]);
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> minimize(const TVector<T, N> &v1, const TVector<T, N> &v2) {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result[i] = std::min(v1[i], v2[i]);
        }

        return result;
    }

    template <typename T, int N> TVector<T, N>::TVector() {
    }

    template <typename T, int N> TVector<T, N>::TVector(T value) {
        for (T &element : this->values) {
            element = value;
        }
    }

    template <typename T, int N> TVector<T, N>::TVector(const T *values) {
        assert(values);

        for (int i = 0; i < N; i++) {
            this->values[i] = values[i];
        }
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator+() const {
        return *this;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator-() const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = -this->values[i];
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator+(const TVector<T, N> &rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] + rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator-(const TVector<T, N> &rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] - rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator*(const TVector<T, N> &rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] * rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator/(const TVector<T, N> &rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] / rhs.values[i];
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator*(const T rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] * rhs;
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> TVector<T, N>::operator/(const T rhs) const {
        TVector<T, N> result;

        for (int i = 0; i < N; i++) {
            result.values[i] = this->values[i] / rhs;
        }

        return result;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator+=(const TVector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] += rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator-=(const TVector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] -= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator*=(const TVector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] *= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator/=(const TVector<T, N> &rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] /= rhs.values[i];
        }

        return *this;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator*=(const T rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] *= rhs;
        }

        return *this;
    }

    template <typename T, int N> TVector<T, N> &TVector<T, N>::operator/=(const T rhs) {
        for (int i = 0; i < N; i++) {
            this->values[i] /= rhs;
        }

        return *this;
    }

    template <typename T, int N> bool TVector<T, N>::operator==(const TVector<T, N> &rhs) const {
        for (int i = 0; i < N; i++) {
            if (this->values[i] != rhs.values[i]) {
                return false;
            }
        }

        return true;
    }

    template <typename T, int N> bool TVector<T, N>::operator!=(const TVector<T, N> &rhs) const {
        return !(*this == rhs);
    }

    template <typename T, int N> inline std::ostream &operator<<(std::ostream &os, const TVector<T, N> &v) {
        os << "xe::TVector<" << typeid(T).name() << ", " << N << ">{ ";

        for (int i = 0; i < N - 1; i++) {
            os << v[i] << ", ";
        }

        os << v[N - 1] << " }";

        return os;
    }

    template <typename T> using TVector2 = TVector<T, 2>;
    template <typename T> using TVector3 = TVector<T, 3>;
    template <typename T> using TVector4 = TVector<T, 4>;

    using Vector2i = TVector2<std::int32_t>;
    using Vector3i = TVector3<std::int32_t>;
    using Vector4i = TVector4<std::int32_t>;

    using Vector2 = TVector2<float>;
    using Vector3 = TVector3<float>;
    using Vector4 = TVector4<float>;

    using Vector2d = TVector2<double>;
    using Vector3d = TVector3<double>;
    using Vector4d = TVector4<double>;

    extern template struct TVector<std::int32_t, 2>;
    extern template struct TVector<std::int32_t, 3>;
    extern template struct TVector<std::int32_t, 4>;

    extern template struct TVector<float, 2>;
    extern template struct TVector<float, 3>;
    extern template struct TVector<float, 4>;

    extern template struct TVector<double, 2>;
    extern template struct TVector<double, 3>;
    extern template struct TVector<double, 4>;
} // namespace xe

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif
