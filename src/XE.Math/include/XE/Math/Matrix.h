
#ifndef __XE_MATH_MATRIX_HPP__
#define __XE_MATH_MATRIX_HPP__

#include <cstdint>
#include <cstring>
#include "Vector.h"

namespace XE {
    template<typename T, int R, int C>
    struct MatrixBase {
        T element[C][R] = {};
    };


    template<typename T, int N>
    struct Vector;

    /**
     * @brief NxM matrix struct, in column-major order.
     */
    template<typename T, int R, int C>
    struct Matrix : public MatrixBase<T, R, C> {
        using MatrixBase<T, R, C>::MatrixBase;

        Matrix() {}

        explicit Matrix(const Vector<T, R*C> &v) {
            constexpr int totalBytes = R * C * sizeof(T);
            std::memcpy(data(), v.data, totalBytes);
        }
        
        explicit Matrix(const T* values) {
            constexpr int totalBytes = R * C * sizeof(T);
            
            assert(values);
            std::memcpy(data(), values, totalBytes);
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

            return this->element[j][i];
        }

        const T& operator() (const int i, const int j) const {
            assert(i >= 0);
            assert(j >= 0);
            assert(i < R);
            assert(j < C);

            return this->element[j][i];
        }

        Vector<T, C> getRow(const int i) const {
            assert(i >= 0);
            assert(i < R);

            Vector<T, C> result;
            
            for (int j=0; j<C; j++) {
                result.data[j] = this->element[j][i];
            }
            
            return result;
        }
        
        Vector<T, R> getColumn(const int j) const {
            assert(j >= 0);
            assert(j < C);

            Vector<T, R> result;
            
            for (int i=0; i<R; i++) {
                result.data[i] = this->element[j][i];
            }
            
            return result;
        }
        
        Matrix<T, R, C>& setRow(const int i, const Vector<T, C> &v) {
            for (int j=0; j<C; j++) {
                this->element[j][i] = v[j];
            }

            return *this;
        }

        Matrix<T, R, C>& setColumn(const int j, const Vector<T, R> &v) {
            for (int i=0; i<R; i++) {
                this->element[j][i] = v[i];
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

        T* data() {
            return &this->element[0][0];
        }
        
        const T* data() const {
            return &this->element[0][0];
        }
        
        static Matrix<T, R, C> make(const std::array<Vector<T, R>, C> &columns) {
            Matrix<T, R, C> result;
            
            for (int i=0; i<R; i++) {
                for (int j=0; j<C; j++) {
                    result.setColumn(j, columns[j]);
                }
            }
            
            return result;
        }

        /**
         * @brief Build a matrix initialized with zeros.
         */
        static Matrix<T, R, C> zero() {
            Matrix<T, R, C> result;
            
            for(int j=0; j<C; ++j) {
                for(int i=0; i<R; ++i) {
                    result(i, j) = static_cast<T>(0);
                }
            }
            
            return result;
        }

        /**
         * @brief Build a identity matrix. Must be square.
         */
        static auto identity() {
            if constexpr (R == C) {
                auto result = Matrix<T, R, C>::zero();
                
                for (int i=0; i<R; ++i) {
                    result(i, i) = static_cast<T>(1);
                }
                
                return result;
            }
        }
        
        static Matrix<T, R, C> scale(const Vector<T, R> &scale) {
            auto result = Matrix<T, R, C>::identity();
            
            for (int i=0; i<R; ++i) {
                result(i, i) = scale[i];
            }
            
            return result;
        }

        static Matrix<T, R, C> translate(const Vector<T, R> &displace) {
            auto result = Matrix<T, R, C>::identity();
            
            result.setRow(R - 1, displace);
            
            return result;
        }

        static Matrix<T, R, C> translate(const Vector<T, R - 1> &displace) {
            return Matrix<T, R, C>::translate({displace, static_cast<T>(1)});
        }

        static Matrix<T, R, C> rotateX(const T radians) {
            auto result = Matrix<T, R, C>::identity();
            
            const T cos = std::cos(radians);
            const T sin = std::sin(radians);
            
            result(1, 1) = cos;
            result(2, 2) = cos;
            
            result(1, 2) = -sin;
            result(2, 1) = sin;
            
            return result;
        }

        static Matrix<T, R, C> rotateY(const T radians) {
            auto result = Matrix<T, R, C>::identity();
            
            const T cos = std::cos(radians);
            const T sin = std::sin(radians);
            
            result(0, 0) = cos;
            result(2, 2) = cos;
            result(0, 2) = sin;
            result(2, 0) = -sin;
            
            return result;
        }

        static Matrix<T, R, C> rotateZ(const T radians) {
            auto result = Matrix<T, R, C>::identity();
            
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
        static auto rotate2(const T rads, const Vector<T, 3> &axis) {
            
            
        }
        
        /**
         * @brief Build a arbitrary rotation matrix 
         */
        static auto rotate(const T rads, const Vector<T, 3> &axis) {
            if constexpr ( (C>=3 && C<=4) && (R>=3 && R<=4) ) {                
                assert(!std::isnan(rads));
                assert(!std::isinf(rads));

                const auto I = Matrix<T, 3, 3>::identity();

                const T cos = std::cos(rads);
                const T sin = std::sin(rads);
                
                Vector<T, 3> U = axis;
                Vector<T, 3> V = normalize(axis);
                
                const auto c1 = Vector<T, 3>{static_cast<T>(0), -V.Z, V.Y};
                const auto c2 = Vector<T, 3>{V.Z, static_cast<T>(0), -V.X};
                const auto c3 = Vector<T, 3>{-V.Y, V.X, static_cast<T>(0)};
                
                const auto matS = Matrix<T, 3, 3>::make({c1, c2, c3});
                
                const auto matUUT = Matrix<T, 3, 1>{V} * Matrix<T, 1, 3>{V};
                const auto tempResult = matUUT + cos * (I - matUUT) + sin * matS;
                
                auto result = Matrix<T, C, R>::identity();

                
                
                for (int i=0; i<3; ++i) {
                    for (int j=0; j<3; ++j) {
                        result(i, j) = tempResult(i, j);
                    }
                }
                
                return result;
            }
        }

        static auto lookAt(const Vector<T, 3> &Eye, const Vector<T, 3> &At, const Vector<T, 3> &Up) {
            if constexpr (C==4 && R==4) {
                const auto forward = normalize(At - Eye);
                const auto side = normalize(cross(forward, Up));
                const auto up = cross(side, forward);
                
                auto result = Matrix<T, 4, 4>::identity();
                
                result(0, 0) = side.X;
                result(0, 1) = side.Y;
                result(0, 2) = side.Z;
                
                result(1, 0) = up.X;
                result(1, 1) = up.Y;
                result(1, 2) = up.Z;
            
                result(2, 0) = -forward.X;
                result(2, 1) = -forward.Y;
                result(2, 2) = -forward.Z;
                
                result = Matrix<T, 4, 4>::translate(-Eye) * result;
                
                return result;
            }
        }

        static auto perspective(const T fov_radians, const T aspect, const T znear, const T zfar) {
            if constexpr (C==4 && R==4) {
                assert(fov_radians > static_cast<T>(0));
                assert(aspect > static_cast<T>(0));
                assert(znear > static_cast<T>(0));

                const T f = static_cast<T>(1) / std::tan(fov_radians / static_cast<T>(2));
                const T zdiff = znear - zfar;

                auto result = Matrix<T, 4, 4>::identity();
                
                result(0, 0) = f / aspect;
                result(1, 1) = f;
                result(2, 2) = (zfar + znear) / zdiff;

                result(2, 3) = static_cast<T>(-1);
                result(3, 2) = (static_cast<T>(2)*znear * zfar) / zdiff;
                
                return result;
            }
        }
        
        static auto orthographic(const Vector<T, 3> &pmin,  const Vector<T, 3> &pmax) {
            if constexpr (C==4 && R==4) {
                const auto diff = pmax - pmin;
                
                auto result = Matrix<T, 4, 4>::createIdentity();
                
                result(0, 0) = static_cast<T>(2) / diff.X;
                result(1, 1) = static_cast<T>(2) / diff.Y;
                result(2, 2) = static_cast<T>(-2) / diff.Z;
                result(3, 3) = static_cast<T>(1);
                
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

    using M3 = Matrix<float, 3, 3>;
    using M4 = Matrix<float, 4, 4>;
    
    template<typename T, int R, int C>
    auto abs(const Matrix<T, R, C> &m) {
        if constexpr (R >= 2) {
            if constexpr (R == 2 && C == 2) {
                return m.element[1][1] * m.element[0][0] - m.element[0][1] * m.element[1][0];
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
                const T factor = ((i+j)%2 == 1) ? static_cast<T>(1) : static_cast<T>(-1);
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
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                if (this->element[i][j] != other.element[i][j]) {
                    return false;
                }
            }
        }
        
        return true;
    }

    template<typename T, int R, int C>
    bool Matrix<T, R, C>::operator!= (const Matrix<T, R, C> &other) const {
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                if (this->element[i][j] == other.element[i][j]) {
                    return false;
                }
            }
        }
        
        return true;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+ (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.element[i][j] = this->element[i][j] + rhs.element[i][j];
            }
        }

        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- () const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.element[i][j] = -this->element[i][j];
            }
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
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.element[i][j] = this->element[i][j] - rhs.element[i][j];
            }
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
                const auto rowI = getRow(i);
                const auto colJ = rhs.getColumn(j);
                const auto mIJ = dot(rowI, colJ);;

                result(i, j) = mIJ;
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
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result(i, j) = (*this)(i, j) * s;
            }
        }

        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const T s) const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result(i, j) = (*this)(i, j) / s;
            }
        }
        
        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                this->element[i][j] += rhs.element[i][j];
            }
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                this->element[i][j] -= rhs.element[i][j];
            }
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
            result[row] = dot(getRow(row), v);
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
