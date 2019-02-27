
#ifndef __XE_MATH_VECTOR_HPP__
#define __XE_MATH_VECTOR_HPP__

#include <cmath>
#include <cstdint>
#include <cassert>
#include <algorithm>
#include <ostream>
#include <string>

namespace XE {
    template<typename T, int N>
    struct VectorBase {
        T data[N];
    };

    template<typename T>
    struct VectorBase<T, 2> {
        union {
            T data[2];
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
            T data[3];
            struct {T X, Y, Z; };
        };

        VectorBase() {}

        VectorBase(T x, T y, T z) {
            X = x;
            Y = y;
            Z = z;
        }

        VectorBase(const VectorBase<T, 2> &v, T z) {
            X = v.x;
            Y = v.y;
            Z = z;
        }

        VectorBase(T x, const VectorBase<T, 2> &v) {
            X = x;
            Y = v.x;
            Z = v.y;
        }
    };
    
    template<typename T>
    struct VectorBase<T, 4> {
        union {
            T data[4];
            struct {T X, Y, Z, W; };
        };

        VectorBase() {}

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
            return this->data[index];
        }

        const T operator[] (const int index) const {
            return this->data[index];
        }

        inline friend Vector<T, N> operator* (const T s, const Vector<T, N>& v) {
            return v * s;
        }

        template<typename T2>
        operator Vector<T2, N>() const {
            Vector<T2, N> result;

            for (int i=0; i<N; i++) {
                result.data[i] = static_cast<T2>(this->data[i]);
            }

            return result;
        }

        template<typename T2>
        Vector<T2, N> cast() const {
            Vector<T2, N> result;

            for (int i=0; i<N; i++) {
                result.data[i] = static_cast<T2>(this->data[i]);
            }

            return result;
        }
    };

    template<typename T, int N>
    T dot(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        T sum = T(0);

        for (int i=0; i<N; i++) {
            sum += v1.data[i]*v2.data[i];
        }

        return sum;
    }

    template<typename T>
    Vector<T, 3> cross(const Vector<T, 3> &v1, const Vector<T, 3> &v2) {
        return {
            v1.Y*v2.Z - v1.Z*v2.Y,
            v1.Z*v2.X - v1.X*v2.Z,
            v1.X*v2.Y - v1.Y*v2.X
        };
    }

    template<typename T>
    T cross(const Vector<T, 2> &v1, const Vector<T, 2> &v2) {
        return v1.X*v2.Y - v1.Y*v2.X;
    }

    template<typename T, int N>
    T norm(const Vector<T, N> &v) {
        return static_cast<T>(std::sqrt(dot(v, v)));
    }

    template<typename T, int N>
    T norm2(const Vector<T, N> &v) {
        return dot(v, v);
    }

    template<typename T, int N>
    Vector<T, N> normalize(const Vector<T, N> &v) {
        return v / norm(v);
    }

    template<typename T, int N>
    Vector<T, N> maximize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result[i] = std::max(v1[i], v2[i]);
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> minimize(const Vector<T, N> &v1, const Vector<T, N> &v2) {
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
        for (T &element : this->data) {
            element = value;
        }
    }

    template<typename T, int N>
    Vector<T, N>::Vector(const T *values) {
        assert(values);

        for (int i=0; i<N; i++) {
            this->data[i] = values[i];
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
            result.data[i] = -this->data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator+ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator- (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] * rhs.data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const Vector<T, N>& rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] / rhs.data[i];
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator* (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] * rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N> Vector<T, N>::operator/ (const T rhs) const {
        Vector<T, N> result;

        for (int i=0; i<N; i++) {
            result.data[i] = this->data[i] / rhs;
        }

        return result;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator+= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] += rhs.data[i];
        }

        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator-= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] -= rhs.data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] *= rhs.data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const Vector<T, N>& rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] /= rhs.data[i];
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator*= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] *= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    Vector<T, N>& Vector<T, N>::operator/= (const T rhs) {
        for (int i=0; i<N; i++) {
            this->data[i] /= rhs;
        }
        
        return *this;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator== (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->data[i] != rhs.data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<typename T, int N>
    bool Vector<T, N>::operator!= (const Vector<T, N>& rhs) const {
        for (int i=0; i<N; i++) {
            if (this->data[i] == rhs.data[i]) {
                return false;
            }
        }
        
        return true;
    }
}

namespace std {
    /**
     * @brief Send a string-representation of the supplied row vector
     */
    template<typename T, int N>
    std::ostream& operator<< (std::ostream &os, const XE::Vector<T, N> &v) {
        os << "[";

        for (int i=0; i<N; i++) {
            os << v[i];

            if (i < N - 1) {
                os << ", ";
            }
        }

        os << "]";

        return os;
    }
}

#endif
