
#ifndef __XE_MATH_MATRIX_HPP__
#define __XE_MATH_MATRIX_HPP__

#include <cstdint>

namespace XE::Math {
    template<typename T, int R, int C>
    struct MatrixBase;

    template<typename T>
    struct MatrixBase<T, 2, 2> {
        union {
            T Data[4];
            T Element[2][2];
            struct {
                T M11, M12;
                T M21, M22;
            };
        };

        MatrixBase(
            const T m11, const T m12,
            const T m21, const T m22
        ) {
            M11 = m11; M12 = m12;
            M21 = m21; M22 = m22;
        }
    };

    template<typename T>
    struct MatrixBase<T, 3, 3> {
        union {
            T Data[9];
            T Element[3][3];
            struct {
                T M11, M12, M13;
                T M21, M22, M23;
                T M31, M32, M33;
            };
        };

        MatrixBase(
            const T m11, const T m12, const T m13,
            const T m21, const T m22, const T m23,
            const T m31, const T m32, const T m33
        ) {
            M11 = m11; M12 = m12; M13 = m13;
            M21 = m21; M22 = m22; M23 = m23;
            M31 = m31; M32 = m32; M33 = m33;
        }
    };
    
    template<typename T>
    struct MatrixBase<T, 4, 4> {
        union {
            T Data[16];
            T Element[4][4];
            struct {
                T M11, M12, M13, M14;
                T M21, M22, M23, M24;
                T M31, M32, M33, M34;
                T M41, M42, M43, M44;
            };
        };

        MatrixBase(
            const T m11, const T m12, const T m13, const T m14,
            const T m21, const T m22, const T m23, const T m24,
            const T m31, const T m32, const T m33, const T m34,
            const T m41, const T m42, const T m43, const T m44
        ) {
            M11 = m11; M12 = m12; M13 = m13; M14 = m14;
            M21 = m21; M22 = m22; M23 = m23; M24 = m24;
            M31 = m31; M32 = m32; M33 = m33; M34 = m34;
            M41 = m41; M42 = m42; M43 = m43; M44 = m44;
        }
    };

    template<typename T, int N>
    struct Vector;

    template<typename T, int R, int C>
    struct Matrix : public MatrixBase<T, R, C> {
        static_assert(R == C);

        using MatrixBase<T, R, C>::MatrixBase;

        Vector<T, C> RowVector(const int row) const;

        Vector<T, R> ColumnVector(const int column) const;

        Matrix<T, R - 1, C - 1> SubMatrix(const int row, const int column) const;

        Matrix<T, C, R> Transpose() const;

        Matrix<T, R, C> Inverse() const;

        Matrix<T, R, C> Inverse(const T determinant) const;

        T Determinant() const;

        Matrix<T, R, C> operator+ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator- (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator* (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator/ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator* (const T s) const;

        Matrix<T, R, C> operator/ (const T s) const;

        Matrix<T, R, C>& operator+= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator-= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator*= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator/= (const Matrix<T, R, C>& rhs);

        inline friend Matrix<T, R, C> operator* (const T s, const Matrix<T, R, C> &m) {
            return m * s;
        }
    };

    typedef Matrix<float, 2, 2> Matrix2f;
    typedef Matrix<float, 3, 3> Matrix3f;
    typedef Matrix<float, 4, 4> Matrix4f;
}

#endif
