
#ifndef __XE_MATH_VECTOR_HPP__
#define __XE_MATH_VECTOR_HPP__

#include <cmath>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace XE::Math {
    template<typename T, int N>
    struct VectorBase {
        T Data[N];
    };

    template<typename T>
    struct VectorBase<T, 2> {
        union {
            T Data[2];
            struct {T X, Y; };
        };

        VectorBase() {}

        VectorBase(T x, T y) {
            X = x;
            Y = y;
        }
    };

    template<typename T>
    struct VectorBase<T, 3> {
        union {
            T Data[3];
            struct {T X, Y, Z; };
        };

        VectorBase() {}

        VectorBase(T x, T y, T z) {
            X = x;
            Y = y;
            Z = z;
        }
    };
    
    template<typename T>
    struct VectorBase<T, 4> {
        union {
            T Data[4];
            struct {T X, Y, Z, W; };
        };

        VectorBase() {}

        VectorBase(T x, T y, T z, T w) {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
    };

    /**
     * @brief N-Dimensional Vector structure.
     */
    template<typename T, int N>
    struct Vector : public VectorBase<T, N> {
        using VectorBase<T, N>::VectorBase;

        explicit Vector();

        explicit Vector(T value);

        explicit Vector(const T *values);

        Vector(const Vector<T, N> &other);

        Vector<T, N> operator+ () const;

        Vector<T, N> operator- () const;

        Vector<T, N> operator+ (const Vector<T, N>& rhs) const;

        Vector<T, N> operator- (const Vector<T, N>& rhs) const;

        Vector<T, N> operator* (const Vector<T, N>& rhs) const;

        Vector<T, N> operator/ (const Vector<T, N>& rhs) const;

        Vector<T, N> operator* (const T rhs) const;

        Vector<T, N> operator/ (const T rhs) const;

        Vector<T, N>& operator+= (const Vector<T, N>& rhs);

        Vector<T, N>& operator-= (const Vector<T, N>& rhs);

        Vector<T, N>& operator*= (const Vector<T, N>& rhs);

        Vector<T, N>& operator/= (const Vector<T, N>& rhs);

        Vector<T, N>& operator*= (const T rhs);

        Vector<T, N>& operator/= (const T rhs);

        bool operator== (const Vector<T, N>& rhs) const;

        bool operator!= (const Vector<T, N>& rhs) const;

        T& operator[] (const int index) {
            return this->Data[index];
        }

        const T operator[] (const int index) const {
            return this->Data[index];
        }

        inline friend Vector<T, N> operator* (const T s, const Vector<T, N>& v) {
            return v * s;
        }
    };

    template<typename T, int N>
    T Dot(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        T sum = T(0);

        for (int i=0; i<N; i++) {
            sum += v1.Data[i]*v2.Data[i];
        }

        return sum;
    }

    template<typename T>
    Vector<T, 3> Cross(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
        return {
            v1.Y*v2.Z - v1.Z*v2.Y,
            v1.Z*v2.X - v1.X*v2.Z,
            v1.X*v2.Y - v1.Y*v2.X
        };
    }

    template<typename T>
    T Cross(const Vector<T, 2> &v1, const Vector<T, 2> &v2) {
        return v1.X*v2.Y - v1.Y*v2.X;
    }

    template<typename T, int N>
    T Abs(const Vector<T, N> &v) {
        return static_cast<T>(std::sqrt(Dot(v, v)));
    }

    template<typename T, int N>
    T Abs_2(const Vector<T, N> &v) {
        return Dot(v, v);
    }

    template<typename T, int N>
    Vector<T, N> Normalize(const Vector<T, N> &v) {
        return v / Abs(v);
    }

    template<typename T, int N>
    Vector<T, N> Maximize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result[i] = std::max(v1[i], v2[i]);
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Minimize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result[i] = std::min(v1[i], v2[i]);
        }

        return result;
    }

    extern template struct Vector<std::int32_t, 2>;
    extern template struct Vector<std::int32_t, 3>;
    extern template struct Vector<std::int32_t, 4>;

    extern template struct Vector<float, 2>;
    extern template struct Vector<float, 3>;
    extern template struct Vector<float, 4>;

    extern template struct Vector<double, 2>;
    extern template struct Vector<double, 3>;
    extern template struct Vector<double, 4>;

    typedef Vector<std::int32_t, 2> Vector2i;
    typedef Vector<std::int32_t, 3> Vector3i;
    typedef Vector<std::int32_t, 4> Vector4i;

    typedef Vector<float, 2> Vector2f;
    typedef Vector<float, 3> Vector3f;
    typedef Vector<float, 4> Vector4f;

    typedef Vector<double, 2> Vector2d;
    typedef Vector<double, 3> Vector3d;
    typedef Vector<double, 4> Vector4d;

    template<typename T, int N>
    Vector<T, N>::Vector() {}

    template<typename T, int N>
    Vector<T, N>::Vector(T value) {
        for (T &element : this->Data) {
            element = value;
        }
    }

    template<typename T, int N>
    Vector<T, N>::Vector(const T *values) {
        assert(values);

        for (int i=0; i<N; i++) {
            this->Data[i] = values[i];
        }
    }

    template<typename T, int N>
    Vector<T, N>::Vector(const Vector<T, N> &other) {
        *this = other;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator+ () const {
        return *this;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator- () const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = -this->Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator+ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] + rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator- (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] - rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] * rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] / rhs.Data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] * rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.Data[i] = this->Data[i] / rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator+= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] += rhs.Data[i];
        }

        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator-= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] -= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] *= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] /= rhs.Data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] *= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->Data[i] /= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator== (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->Data[i] != rhs.Data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator!= (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->Data[i] == rhs.Data[i]) {
                return false;
            }
        }
        
        return true;
    }
}

#endif
