
#ifndef __XE_MATH_MATRIX_HPP__
#define __XE_MATH_MATRIX_HPP__

#include <cstdint>
#include <cstring>
#include "Vector.h"

namespace XE {
    template<typename T, int R, int C>
    struct MatrixBase {
        union {
            T data[R*C];
            T element[R][C];
        };
    };

    template<typename T>
    struct MatrixBase<T, 2, 2> {
        union {
            T data[4];
            T element[2][2];
            struct {
                T M11, M12;
                T M21, M22;
            };
        };

        MatrixBase() {}

        MatrixBase(
            const T m11, const T m12,
            const T m21, const T m22
        ) {
            M11 = m11; M12 = m12;
            M21 = m21; M22 = m22;
        }

        MatrixBase(
            const Vector<T, 2> &row1,
            const Vector<T, 2> &row2
        ) {
            M11 = row1.X; M12 = row1.Y;
            M21 = row2.X; M22 = row2.Y;
        }
    };

    template<typename T>
    struct MatrixBase<T, 3, 3> {
        union {
            T data[9];
            T element[3][3];
            struct {
                T M11, M12, M13;
                T M21, M22, M23;
                T M31, M32, M33;
            };
        };

        MatrixBase() {}

        MatrixBase(
            const T m11, const T m12, const T m13,
            const T m21, const T m22, const T m23,
            const T m31, const T m32, const T m33
        ) {
            M11 = m11; M12 = m12; M13 = m13;
            M21 = m21; M22 = m22; M23 = m23;
            M31 = m31; M32 = m32; M33 = m33;
        }

        MatrixBase(
            const Vector<T, 3> &row1,
            const Vector<T, 3> &row2,
            const Vector<T, 3> &row3
        ) {
            M11 = row1.X; M12 = row1.Y; M13 = row1.Z;
            M21 = row2.X; M22 = row2.Y; M23 = row2.Z;
            M31 = row3.X; M32 = row3.Y; M33 = row3.Z;
        }
    };
    
    template<typename T>
    struct MatrixBase<T, 4, 4> {
        union {
            T data[16];
            T element[4][4];
            struct {
                T M11, M12, M13, M14;
                T M21, M22, M23, M24;
                T M31, M32, M33, M34;
                T M41, M42, M43, M44;
            };
        };

        MatrixBase() {}

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

        MatrixBase(
            const Vector<T, 4> &row1,
            const Vector<T, 4> &row2,
            const Vector<T, 4> &row3,
            const Vector<T, 4> &row4
        ) {
            M11 = row1.X; M12 = row1.Y; M13 = row1.Z; M14 = row1.W;
            M21 = row2.X; M22 = row2.Y; M23 = row2.Z; M24 = row2.W;
            M31 = row3.X; M32 = row3.Y; M33 = row3.Z; M34 = row3.W;
            M41 = row4.X; M42 = row4.Y; M43 = row4.Z; M44 = row4.W;
        }
    };

    template<typename T, int N>
    struct Vector;

    template<typename T, int R, int C>
    struct Matrix : public MatrixBase<T, R, C> {
        using MatrixBase<T, R, C>::MatrixBase;

        Matrix() {}

        Matrix(const Vector<T, R*C> &v) {
            constexpr int totalBytes = R * C * sizeof(T);
            std::memcpy(this->data, v.data, totalBytes);
        }

        bool operator== (const Matrix<T, R, C> &other) const;

        bool operator!= (const Matrix<T, R, C> &other) const;

        Matrix<T, R, C> operator+ () const;

        Matrix<T, R, C> operator+ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator- () const;

        Matrix<T, R, C> operator- (const Matrix<T, R, C>& rhs) const;

        template<int R2, int C2>
        Matrix<T, R, C2> operator* (const Matrix<T, R2, C2>& rhs) const;

        Matrix<T, R, C> operator/ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator* (const T s) const;

        Matrix<T, R, C> operator/ (const T s) const;

        Matrix<T, R, C>& operator+= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator-= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator*= (const Matrix<T, R, C>& rhs);

        Vector<T, R> operator* (const Vector<T, R> &v) const;

        inline friend Matrix<T, R, C> operator* (const T s, const Matrix<T, R, C> &m) {
            return m * s;
        }

        T& operator() (const int i, const int j) {
            assert(i >= 0);
            assert(j >= 0);
            assert(i < R);
            assert(j < C);

            return this->element[i][j];
        }

        T operator() (const int i, const int j) const {
            assert(i >= 0);
            assert(j >= 0);
            assert(i < R);
            assert(j < C);

            return this->element[i][j];
        }

        Vector<T, C> getRow(const int row) const {
            assert(row >= 0);
            assert(row < R);

            Vector<T, C> result;
            
            for (int col=0; col<C; col++) {
                result.data[col] = this->element[row][col];
            }
            
            return result;
        }
        
        Vector<T, R> getColumn(const int col) const {
            assert(col >= 0);
            assert(col < C);

            Vector<T, R> result;
            
            for (int row=0; row<R; row++) {
                result.data[row] = this->element[row][col];
            }
            
            return result;
        }
        
        Matrix<T, R, C>& setRow(const int row, const Vector<T, C> &v) {
            for (int col=0; col<C; col++) {
                this->element[row][col] = v[col];
            }

            return *this;
        }

        Matrix<T, R, C>& setColumn(const int col, const Vector<T, R> &v) {
            for (int row=0; row<R; row++) {
                this->element[row][col] = v[row];
            }

            return *this;
        }

        auto getSubMatrix(const int row, const int column) const {
            if constexpr (C > 2 && R > 2) {
                assert(row >= 0);
                assert(row < R);

                assert(column >= 0);
                assert(column < C);

                Matrix<T, R - 1, C - 1> result;
                
                int ii = 0, jj = 0;
                
                for (int i=0; i<R; ++i) {
                    if (i == row) {
                        continue;
                    }
                    
                    for (int j=0; j<C; ++j) {
                        if (j == column) {
                            continue;
                        }
                        
                        result(ii, jj) = (*this)(i, j);
                        ++jj;
                    }
                    
                    ++ii;
                    jj = 0;
                }
                
                return result;
            }
        }

        /**
         * @brief Build a matrix initialized with zeros.
         */
        static Matrix<T, R, C> createZero() {
            Matrix<T, R, C> result;
            
            for(int i=0; i<R*C; ++i) {
                result.data[i] = T(0);
            }
            
            return result;
        }
        
        /**
         * @brief Build a identity matrix. Must be square.
         */
        static auto createIdentity() {
            if constexpr (R == C) {
                auto result = Matrix<T, R, C>::createZero();
                
                for (int i=0; i<R; ++i) {
                    result(i, i) = T(1);
                }
                
                return result;
            }
        }
        
        static Matrix<T, R, C> createScaling(const Vector<T, R> &scale) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            for (int i=0; i<R; ++i) {
                result(i, i) = scale[i];
            }
            
            return result;
        }
        
        static Matrix<T, R, C> createTranslation(const Vector<T, R> &displace) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            result.setColumn(C - 1, displace);
            
            return result;
        }

        static Matrix<T, R, C> createTranslation(const Vector<T, R - 1> &displace) {
            return Matrix<T, R, C>::createTranslation({displace, T(1)});
        }

        static Matrix<T, R, C> createRotationX(const T radians) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            const T cos = std::cos(radians);
            const T sin = std::sin(radians);
            
            result(1, 1) = cos;
            result(2, 2) = cos;
            result(1, 2) = -sin;
            result(2, 1) = sin;
            
            return result;
        }
        
        static Matrix<T, R, C> createRotationY(const T radians) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            const T cos = std::cos(radians);
            const T sin = std::sin(radians);
            
            result(0, 0) = cos;
            result(2, 2) = cos;
            result(0, 2) = sin;
            result(2, 0) = -sin;
            
            return result;
        }
        
        static Matrix<T, R, C> createRotationZ(const T radians) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            const T cos = std::cos(radians);
            const T sin = std::sin(radians);
            
            result(0, 0) = cos;
            result(1, 1) = cos;
            result(0, 1) = -sin;
            result(1, 0) = sin;
            
            return result;
        }
        
        /**
         * @brief Build a arbitrary rotation matrix 
         */
        static auto createRotation(const T rads, const Vector<T, 3> &axis) {
            if constexpr ( (C>=3 && C<=4) && (R>=3 && R<=4) ) {                
                assert(!std::isnan(rads));
                assert(!std::isinf(rads));

                const auto I = Matrix<T, 3, 3>::createIdentity();

                const T cos = std::cos(rads);
                const T sin = std::sin(rads);
                
                Vector<T, 3> U = axis;
                Vector<T, 3> V = normalize(axis);
                
                const Matrix<T, 3, 3> matS = {
                    T(0), -V.Z, V.Y,
                    V.Z , T(0), -V.X,
                    -V.Y, V.X , T(0),
                };
                
                const auto matUUT = Matrix<T, 3, 1>{V} * Matrix<T, 1, 3>{V};
                const auto tempResult = matUUT + cos * (I - matUUT) + sin * matS;
                
                auto result = Matrix<T, C, R>::createIdentity();

                for (int i=0; i<3; ++i) {
                    for (int j=0; j<3; ++j) {
                        result(i, j) = tempResult(i, j);
                    }
                }
                
                return result;
            }
        }
        
        static auto createLookAt(const Vector<T, 3> &Eye, const Vector<T, 3> &At, const Vector<T, 3> &Up) {
            if constexpr (C==4 && R==4) {
                const auto forward = normalize(At - Eye);
                const auto side = normalize(cross(forward, Up));
                const auto up = cross(side, forward);
                
                auto result = Matrix<T, 4, 4>::createIdentity();
                
                result(0, 0) = side.X;
                result(0, 1) = side.Y;
                result(0, 2) = side.Z;
            
                result(1, 0) = up.X;
                result(1, 1) = up.Y;
                result(1, 2) = up.Z;
            
                result(2, 0) = -forward.X;
                result(2, 1) = -forward.Y;
                result(2, 2) = -forward.Z;
                
                result *= Matrix<T, 4, 4>::createTranslation(-Eye);
                
                return result;
            }
        }
        
        static auto createPerspective(const T fov_radians, const T aspect, const T znear, const T zfar) {
            if constexpr (C==4 && R==4) {
                const T f = T(1) / std::tan(fov_radians / T(2));
                const T zdiff = znear - zfar;

                auto result = Matrix<T, 4, 4>::createIdentity();
                
                result(0, 0) = f / aspect;
                result(1, 1) = f;
                result(2, 2) = (zfar + znear) / zdiff;
                result(3, 2) = T(-1);
                result(2, 3) = (T(2)*znear * zfar) / zdiff;
                
                return result;
            }
        }
        
        static auto createOrthographic(const Vector<T, 3> &pmin,  const Vector<T, 3> &pmax) {
            if constexpr (C==4 && R==4) {
                const auto diff = pmax - pmin;
                
                auto result = Matrix<T, 4, 4>::createIdentity();
                
                result(0, 0) = T(2) / diff.X;
                result(1, 1) = T(2) / diff.Y;
                result(2, 2) = T(-2) / diff.Z;
                result(3, 3) = T(1);
                
                result(0, 3) = -(pmax.X + pmin.X ) / diff.X;
                result(1, 3) = -(pmax.Y + pmin.Y ) / diff.Y;
                result(2, 3) = -(pmax.Z + pmin.Z ) / diff.Z;
                
                return result;
            }
        }
    };
    
    typedef Matrix<float, 2, 2> Matrix2f;
    typedef Matrix<float, 3, 3> Matrix3f;
    typedef Matrix<float, 4, 4> Matrix4f;
    
    template<typename T, int R, int C>
    auto abs(const Matrix<T, R, C> &m) {
        if constexpr (R >= 2) {
            if constexpr (R == 2 && C == 2) {
                return m.M22 * m.M11 - m.M12 * m.M21;
            } else {
                T factor = T(1);
                T result = T(0);
            
                const int row = 0;
            
                for (int column=0; column<C; column++) {
                    factor = (column + 1)%2 ? T(-1) : T(1); 
                
                    const T subDet = abs(m.getSubMatrix(row, column));
                
                    result += factor * m.element[row][column] * subDet;
                }
            
                return result;
            }
        }
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> adjoint(const Matrix<T, R, C> &matrix) {
        Matrix<T, R, C> result;
        
        for(int i=0; i<R; ++i) {
            for(int j=0; j<C; ++j) {
                T factor = ((i+j)%2 == 1) ? T(1) : T(-1);
                result(i, j) = factor * abs(matrix.getSubMatrix(i, j));
            }
        }
        
        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, C, R> transpose(const Matrix<T, R, C> &m) {
        Matrix<T, C, R> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.element[j][i] = m.element[i][j];
            }
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> inverse(const Matrix<T, R, C> &m) {
        return transpose(adjoint(m)) / abs(m);
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> inverse(const Matrix<T, R, C> &m, const T abs) {
        return transpose(adjoint(m)) / abs;
    }
    
    template<typename T, int R, int C>
    bool Matrix<T, R, C>::operator== (const Matrix<T, R, C> &other) const {
        for (int i=0; i<R*C; i++) {
            if (this->data[i] != other.data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<typename T, int R, int C>
    bool Matrix<T, R, C>::operator!= (const Matrix<T, R, C> &other) const {
        for (int i=0; i<R*C; i++) {
            if (this->data[i] == other.data[i]) {
                return false;
            }
        }
        
        return true;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+ (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.data[i] = this->data[i] + rhs.data[i];
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- () const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.data[i] = -this->data[i];
        }
        
        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+ () const {
        return *this;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.data[i] = this->data[i] - rhs.data[i];
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    template<int R2, int C2>
    Matrix<T, R, C2> Matrix<T, R, C>::operator* (const Matrix<T, R2, C2>& rhs) const {
        static_assert(R == C2);
        static_assert(C == R2);
        
        Matrix<T, R, C2> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C2; j++) {
                const auto rowI = this->getRow(i);
                const auto colJ = rhs.getColumn(j);
                const auto mIJ = dot(rowI, colJ);;

                result.element[i][j] = mIJ;
            }
        }
        
        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const Matrix<T, R, C>& rhs) const {
        return *this * inverse(rhs);
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator* (const T s) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.data[i] = this->data[i] * s;
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const T s) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.data[i] = this->data[i] / s;
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            this->data[i] += rhs.data[i];
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            this->data[i] -= rhs.data[i];
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator*= (const Matrix<T, R, C>& rhs) {
        *this = *this * rhs;
        
        return *this;
    }

    template<typename T, int R, int C>
    Vector<T, R> Matrix<T, R, C>::operator* (const Vector<T, R> &v) const {
        Vector<T, R> result;

        for (int row=0; row<R; row++) {
            result[row] = dot(this->getRow(row), v);
        }

        return result;
    }

    template<typename T, int R, int C>
    Vector<T, C> operator* (const Vector<T, C> &v, const Matrix<T, R, C> &m) {
        Vector<T, C> result;

        for (int col=0; col<C; col++) {
            result[col] = dot(m.getColumn(col), v);
        }

        return result;
    }
}

#endif
