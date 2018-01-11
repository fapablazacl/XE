
#ifndef __XE_MATH_MATRIX_HPP__
#define __XE_MATH_MATRIX_HPP__

#include <cstdint>
#include "Vector.hpp"

namespace XE::Math {
    template<typename T, int R, int C>
    struct MatrixBase {
        union {
            T Data[R*C];
            T Element[R][C];
        };
    };

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

        MatrixBase() {}

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
    };

    template<typename T, int N>
    struct Vector;

    template<typename T, int R, int C>
    struct Matrix : public MatrixBase<T, R, C> {
        static_assert(R == C);

        using MatrixBase<T, R, C>::MatrixBase;

        Matrix() {}

        Matrix<T, R, C> operator+ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator- (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator* (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator/ (const Matrix<T, R, C>& rhs) const;

        Matrix<T, R, C> operator* (const T s) const;

        Matrix<T, R, C> operator/ (const T s) const;

        Matrix<T, R, C>& operator+= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator-= (const Matrix<T, R, C>& rhs);

        Matrix<T, R, C>& operator*= (const Matrix<T, R, C>& rhs);

        inline friend Matrix<T, R, C> operator* (const T s, const Matrix<T, R, C> &m) {
            return m * s;
        }

        T& operator() (const int i, const int j) {
            return this->Element[i][j];
        }

        T operator() (const int i, const int j) const {
            return this->Element[i][j];
        }

        Vector<T, C> GetRowVector(const int row) const {
            Vector<T, C> result;
            
            for (int col=0; col<C; col++) {
                result.Data[col] = this->Element[row][col];
            }
            
            return result;
        }
        
        Vector<T, R> GetColumnVector(const int col) const {
            Vector<T, R> result;
            
            for (int row=0; row<R; row++) {
                result.Data[row] = this->Element[row][col];
            }
            
            return result;
        }
        
        void SetRowVector(const int row, const Vector<T, C> &v) {
            for (int col=0; col<C; col++) {
                this->Element[row][col] = v[col];
            }
        }

        void SetColumnVector(const int col, const Vector<T, R> &v) {
            for (int row=0; row<R; row++) {
                this->Element[row][col] = v[col];
            }
        }

        auto SubMatrix(const int row, const int column) {
            if constexpr (C > 2 && R > 2) {
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
        static Matrix<T, R, C> Zero() {
            Matrix<T, R, C> result;
            
            for(int i=0; i<R*C; ++i) {
                result.Data[i] = T(0);
            }
            
            return result;
        }
        
        /**
         * @brief Build a identity matrix. Must be square.
         */
        static auto Identity() {
            if constexpr (R == C) {
                auto result = Matrix<T, R, C>::Zero();
                
                for (int i=0; i<R; ++i) {
                    result.Data[i] = T(1);
                }
                
                return result;
            }
        }
        
        static Matrix<T, R, C> Scale(const Vector<T, R> &scale) {
            auto result = Matrix<T, R, C>::Zero();
            
            for (int i=0; i<R; ++i) {
                result(i, i) = scale[i];
            }
            
            return result;
        }
        
        static Matrix<T, R, C> Translate(const Vector<T, R> &displace) {
            auto result = Matrix<T, R, C>::Zero();
            
            result.SetColumnVector(C - 1, displace);
            
            return result;
        }

        static Matrix<T, R, C> Translate(const Vector<T, R - 1> &displace) {
            auto result = Matrix<T, R, C>::Zero();
            
            Vector<T, R> d;

            for (int i=0; i<R - 1; i++) {
                d[i] = displace[i];
            }

            d[R - 1] = T(1);

            result.SetColumnVector(C - 1, d);
            
            return result;
        }

        static Matrix<T, R, C> RotateX(const T radians) {
            auto result = Matrix<T, R, C>::Identity();
            
            T Cos = std::cos(radians);
            T Sin = std::sin(radians);
            
            result(1, 1) = Cos;
            result(2, 2) = Cos;
            result(2, 1) = -Sin;
            result(1, 2) = Sin;
            
            return result;
        }
        
        static Matrix<T, R, C> RotateY(const T radians) {
            auto result = Matrix<T, R, C>::Identity();
            
            T Cos = std::cos(radians);
            T Sin = std::sin(radians);
            
            result(0, 0) = Cos;
            result(2, 2) = Cos;
            result(2, 0) = -Sin;
            result(0, 2) = Sin;
            
            return result;
        }
        
        static Matrix<T, R, C> RotateZ(const T radians) {
            auto result = Matrix<T, R, C>::Identity();
            
            T Cos = std::cos(radians);
            T Sin = std::sin(radians);
            
            result(0, 0) = Cos;
            result(1, 1) = Cos;
            result(1, 0) = Sin;
            result(0, 1) = -Sin;
            
            return result;
        }
        
        /**
         * @brief Build a arbitrary rotation matrix 
         */
        static auto Rotate(T radians, const Vector<T, 3> &Axis) {
            if constexpr ( (C>=3 && C<=4) && (R>=3 && R<=4) ) {
                assert(!std::isnan(radians));
                assert(!std::isinf(radians));

                T Cos = std::cos(radians);
                T Sin = std::sin(radians);
                
                Vector<T, 3> U = Axis;
                Vector<T, 3> V = Normalize(Axis);
                
                //auto MatS = Matrix<T, 3, 3>::makeZero();
                auto MatUut = Matrix<T, 3, 3>::Zero();
                auto MatId = Matrix<T, 3, 3>::Identity();
                
                //Iniciar S
                Matrix<T, 3, 3> MatS = {
                    T(0), -V.Z, V.Y,
                    V.Z , T(0), -V.X,
                    -V.Y, V.X , T(0),
                };
                
                //auto matU = Matrix<T, 1, 3>{V};
                //auto matU_Ut = transpose(matU) * matU;
                
                //Iniciar u*ut
                MatUut(0, 0) = V.X * V.X;
                MatUut(1, 0) = V.Y * V.X;
                MatUut(2, 0) = V.Z * V.X;
            
                MatUut(0, 1) = V.X * V.Y;
                MatUut(1, 1) = V.Y * V.Y;
                MatUut(2, 1) = V.Z * V.Y;
                
                MatUut(0, 2) = V.X * V.Z;
                MatUut(1, 2) = V.Y * V.Z;
                MatUut(2, 2) = V.Z * V.Z;
                
                auto tempResult = MatUut + Cos * (MatId - MatUut) + Sin * MatS;
                
                auto result = Matrix<T, C, R>::Identity();
                
                for (int i=0; i<3; ++i) {
                    for (int j=0; j<3; ++j) {
                        result(i, j) = tempResult(i, j);
                    }
                }
                
                return result;
            }
        }
        
        static auto Lookat(const Vector<T, 3> &Eye, const Vector<T, 3> &At, const Vector<T, 3> &Up) {
            if constexpr (C==4 && R==4) {
                const auto forward = Normalize(At - Eye);
                const auto side = Normalize(Cross(forward, Up));
                const auto up = Cross(side, forward);
                
                auto result = Matrix<T, 4, 4>::Identity();
                
                result(0, 0) = side.X;
                result(0, 1) = side.Y;
                result(0, 2) = side.Z;
            
                result(1, 0) = up.X;
                result(1, 1) = up.Y;
                result(1, 2) = up.Z;
            
                result(2, 0) = -forward.X;
                result(2, 1) = -forward.Y;
                result(2, 2) = -forward.Z;
                
                result *= Matrix<T, 4, 4>::Translate(-Eye);
                
                return result;
            }
        }
        
        static auto Perspective(T fov_radians, T aspect, T znear, T zfar) {
            if constexpr (C==4 && R==4) {
                const T f = T(1) / std::tan(fov_radians / T(2));
                const T zdiff = znear - zfar;

                auto result = Matrix<T, 4, 4>::Identity();
                
                result(0, 0) = f / aspect;
                result(1, 1) = f;
                result(2, 2) = (zfar + znear) / zdiff;
                result(3, 2) = T(-1);
                result(2, 3) = (T(2)*znear * zfar) / zdiff;
                
                return result;
            }
        }
        
        static auto Ortho(const Vector<T, 3> &pmin,  const Vector<T, 3> &pmax) {
            if constexpr (C==4 && R==4) {
                auto diff = pmax - pmin;
                auto result = Matrix<T, 4, 4>::Identity();
                
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
    
    extern template struct Matrix<float, 2, 2>;
    extern template struct Matrix<float, 3, 3>;
    extern template struct Matrix<float, 4, 4>;

    typedef Matrix<float, 3, 3> Matrix3f;
    typedef Matrix<float, 4, 4> Matrix4f;
    
    template<typename T, int R, int C>
    T Abs(const Matrix<T, R, C> &m) {
        static_assert(R == C);
        static_assert(R >= 2);
        
        if constexpr (R == 2 && C == 2) {
            return m.M22 * m.M11 - m.M12 * m.M21;
        } else {
            T factor = T(1);
            T result = T(0);
            
            const int row = 0;
            
            for (int column=0; column<C; column++) {
                factor = (column + 1)%2 ? T(-1) : T(1);
                
                const T subDet = Abs(SubMatrix(m, row, column));
                
                result += factor * m.Element[row][column] * subDet;
            }
            
            return result;
        }
    }
    
    template<typename T, int R, int C>
    Matrix<T, C, R> Transpose(const Matrix<T, R, C> &m) {
        Matrix<T, C, R> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.Element[j][i] = m.Element[i][j];
            }
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Inverse(const Matrix<T, R, C> &m) {
        return Matrix<T, R, C>();
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Inverse(const Matrix<T, R, C> &m, const T abs) {
        return Matrix<T, R, C>();
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+ (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.Data[i] = this->Data[i] + rhs.Data[i];
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.Data[i] = this->Data[i] - rhs.Data[i];
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator* (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.Element[i][j] = Dot(this->GetRowVector(i), rhs.GetColumnVector(j));
            }
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const Matrix<T, R, C>& rhs) const {
        return *this * Inverse(rhs);
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator* (const T s) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.Data[i] = this->Data[i] * s;
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const T s) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R*C; i++) {
            result.Data[i] = this->Data[i] / s;
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            this->Data[i] += rhs.Data[i];
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            this->Data[i] -= rhs.Data[i];
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator*= (const Matrix<T, R, C>& rhs) {
        *this = *this * rhs;
        
        return *this;
    }
}

#endif
