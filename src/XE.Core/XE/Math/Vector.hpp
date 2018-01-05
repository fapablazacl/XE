
#ifndef __XE_MATH_VECTOR_HPP__
#define __XE_MATH_VECTOR_HPP__

#include <cstdint>

namespace XE::Math {
    template<typename T, int N>
    struct VectorBase;

    template<typename T>
    struct VectorBase<T, 2> {
        union {
            T Data[2];
            struct {T X, Y; };
        };

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
    struct Vector : VectorBase<T, N> {
        VectorBase<T, N>::VectorBase;

        explicit Vector();

        explicit Vector(T value);

        explicit Vector(const T *values);

        Vector<T, N> operator+ () const;

        Vector<T, N> operator- () const;

        Vector<T, N> operator+ (const Vector<T, N>& rhs) const;

        Vector<T, N> operator- (const Vector<T, N>& rhs) const;

        Vector<T, N> operator* (const Vector<T, N>& rhs) const;

        Vector<T, N> operator/ (const Vector<T, N>& rhs) const;

        Vector<T, N>& operator+= (const Vector<T, N>& rhs);

        Vector<T, N>& operator-= (const Vector<T, N>& rhs);

        Vector<T, N>& operator*= (const Vector<T, N>& rhs);

        Vector<T, N>& operator/= (const Vector<T, N>& rhs);

        bool operator== (const Vector<T, N>& rhs) const;

        bool operator!= (const Vector<T, N>& rhs) const;
    };

    typedef Vector<std::int32_t, 2> Vector2i;
    typedef Vector<std::int32_t, 3> Vector3i;
    typedef Vector<std::int32_t, 4> Vector4i;

    typedef Vector<float, 2> Vector2f;
    typedef Vector<float, 3> Vector3f;
    typedef Vector<float, 4> Vector4f;

    typedef Vector<double, 2> Vector2d;
    typedef Vector<double, 3> Vector3d;
    typedef Vector<double, 4> Vector4d;
}

#endif
