
#ifndef __XE_MATH_MATRIX_HPP__
#define __XE_MATH_MATRIX_HPP__

#include <cstdint>
#include <cstring>
#include "Vector.h"

namespace XE {
    template<typename T, int R, int C>
    struct MatrixBase {
        T element[C][R];
    };

    template<typename T>
    struct MatrixBase<T, 2, 2> {
        T element[2][2];

        MatrixBase() {}

        [[deprecated]]
        MatrixBase(
            const T m11, const T m12,
            const T m21, const T m22
        ) {
            element[0][0] = m11; element[0][1] = m12;
            element[1][0] = m21; element[1][1] = m22;
        }

        [[deprecated]]
        MatrixBase(
            const Vector<T, 2> &row1,
            const Vector<T, 2> &row2
        ) {
            element[0][0] = row1.X; element[0][1] = row1.Y;
            element[1][0] = row2.X; element[1][1] = row2.Y;
        }
    };

    template<typename T>
    struct MatrixBase<T, 3, 3> {
        T element[3][3];

        MatrixBase() {}

        [[deprecated]]
        MatrixBase(
            const T m11, const T m12, const T m13,
            const T m21, const T m22, const T m23,
            const T m31, const T m32, const T m33
        ) {
            element[0][0] = m11; element[0][1] = m12; element[0][2] = m13;
            element[1][0] = m21; element[1][1] = m22; element[1][2] = m23;
            element[2][0] = m31; element[2][1] = m32; element[2][2] = m33;
        }

        [[deprecated]]
        MatrixBase(
            const Vector<T, 3> &col1,
            const Vector<T, 3> &col2,
            const Vector<T, 3> &col3
        ) {
            element[0][0] = col1.X; element[0][1] = col1.Y; element[0][2] = col1.Z;
            element[1][0] = col2.X; element[1][1] = col2.Y; element[1][2] = col2.Z;
            element[2][0] = col3.X; element[2][1] = col3.Y; element[2][2] = col3.Z;
        }
    };
    
    template<typename T>
    struct MatrixBase<T, 4, 4> {
        T element[4][4];

        MatrixBase() {}

        [[deprecated]]
        MatrixBase(
            const T m11, const T m12, const T m13, const T m14,
            const T m21, const T m22, const T m23, const T m24,
            const T m31, const T m32, const T m33, const T m34,
            const T m41, const T m42, const T m43, const T m44
        ) {
            element[0][0] = m11; element[0][1] = m12; element[0][2] = m13; element[0][3] = m14;
            element[1][0] = m21; element[1][1] = m22; element[1][2] = m23; element[1][3] = m24;
            element[2][0] = m31; element[2][1] = m32; element[2][2] = m33; element[2][3] = m34;
            element[3][0] = m41; element[3][1] = m42; element[3][2] = m43; element[3][3] = m44;
        }

        [[deprecated]]
        MatrixBase(
            const Vector<T, 4> &col1,
            const Vector<T, 4> &col2,
            const Vector<T, 4> &col3,
            const Vector<T, 4> &col4
        ) {
            element[0][0] = col1.X; element[0][1] = col1.Y; element[0][2] = col1.Z; element[0][3] = col1.W;
            element[1][0] = col2.X; element[1][1] = col2.Y; element[1][2] = col2.Z; element[1][3] = col2.W;
            element[2][0] = col3.X; element[2][1] = col3.Y; element[2][2] = col3.Z; element[2][3] = col3.W;
            element[3][0] = col4.X; element[3][1] = col4.Y; element[3][2] = col4.Z; element[3][3] = col4.W;
        }
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

        [[deprecated]]
        Matrix(const Vector<T, R*C> &v) {
            constexpr int totalBytes = R * C * sizeof(T);
            std::memcpy(data(), v.data, totalBytes);
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

            return element[j][i];
        }

        T operator() (const int i, const int j) const {
            assert(i >= 0);
            assert(j >= 0);
            assert(i < R);
            assert(j < C);

            return element[j][i];
        }

        Vector<T, C> getRow(const int i) const {
            assert(i >= 0);
            assert(i < R);

            Vector<T, C> result;
            
            for (int j=0; j<C; j++) {
                result.data[j] = element[j][i];
            }
            
            return result;
        }
        
        Vector<T, R> getColumn(const int j) const {
            assert(j >= 0);
            assert(j < C);

            Vector<T, R> result;
            
            for (int i=0; i<R; i++) {
                result.data[i] = element[j][i];
            }
            
            return result;
        }
        
        Matrix<T, R, C>& setRow(const int i, const Vector<T, C> &v) {
            for (int j=0; j<C; j++) {
                element[j][i] = v[j];
            }

            return *this;
        }

        Matrix<T, R, C>& setColumn(const int j, const Vector<T, R> &v) {
            for (int i=0; i<R; i++) {
                element[j][i] = v[i];
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
            return &element[0][0];
        }
        
        const T* data() const {
            return &element[0][0];
        }

        /**
         * @brief Build a matrix initialized with zeros.
         */
        [[deprecated]]
        static Matrix<T, R, C> createZero() {
            Matrix<T, R, C> result;
            
            for(int j=0; j<C; ++j) {
                for(int i=0; i<R; ++i) {
                    result(i, j) = static_cast<T>(0);
                }
            }
            
            return result;
        }
        
        static Matrix<T, R, C> zero() {
            return createZero();
        }

        /**
         * @brief Build a identity matrix. Must be square.
         */
        [[deprecated]]
        static auto createIdentity() {
            if constexpr (R == C) {
                auto result = Matrix<T, R, C>::createZero();
                
                for (int i=0; i<R; ++i) {
                    result(i, i) = static_cast<T>(1);
                }
                
                return result;
            }
        }
        
        static auto identity() {
            return createIdentity();
        }

        [[deprecated]]
        static Matrix<T, R, C> createScaling(const Vector<T, R> &scale) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            for (int i=0; i<R; ++i) {
                result(i, i) = scale[i];
            }
            
            return result;
        }
        
        static auto scale(const Vector<T, R> &scale) {
            return createScaling(scale);
        }

        [[deprecated]]
        static Matrix<T, R, C> createTranslation(const Vector<T, R> &displace) {
            auto result = Matrix<T, R, C>::createIdentity();
            
            result.setRow(R - 1, displace);
            
            return result;
        }

        static Matrix<T, R, C> translate(const Vector<T, R> &displace) {
            return createTranslation(displace);
        }

        [[deprecated]]
        static Matrix<T, R, C> createTranslation(const Vector<T, R - 1> &displace) {
            return Matrix<T, R, C>::createTranslation({displace, static_cast<T>(1)});
        }
        
        static Matrix<T, R, C> translate(const Vector<T, R - 1> &displace) {
            return createTranslation(displace);
        }

        [[deprecated]]
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
                
        static Matrix<T, R, C> rotateX(const T radians) {
            return createRotationX(radians);
        }

        [[deprecated]]
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
                
        static Matrix<T, R, C> rotateY(const T radians) {
            return createRotationY(radians);
        }

        [[deprecated]]
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
        
        static Matrix<T, R, C> rotateZ(const T radians) {
            return createRotationZ(radians);
        }

        /**
         * @brief Build a arbitrary rotation matrix 
         */
        [[deprecated]]
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
        
        static auto rotate(const T rads, const Vector<T, 3> &axis) {
            return createRotation(rads, axis);
        }

        [[deprecated]]
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
                
                result = Matrix<T, 4, 4>::createTranslation(-Eye) * result;
                
                return result;
            }
        }

        static auto lookAt(const Vector<T, 3> &Eye, const Vector<T, 3> &At, const Vector<T, 3> &Up) {
            return createLookAt(Eye, At, Up);
        }
        
        [[deprecated]]
        static auto createPerspective(const T fov_radians, const T aspect, const T znear, const T zfar) {
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
        
        static auto perspective(const T fov_radians, const T aspect, const T znear, const T zfar) {
            return createPerspective(fov_radians, aspect, znear, zfar);
        }

        [[deprecated]]
        static auto createOrthographic(const Vector<T, 3> &pmin,  const Vector<T, 3> &pmax) {
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

        static auto orthographic(const Vector<T, 3> &pmin,  const Vector<T, 3> &pmax) {
            return createOrthographic(pmin, pmax);
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
                if (element[i][j] != other.element[i][j]) {
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
                if (element[i][j] == other.element[i][j]) {
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
                result.element[i][j] = element[i][j] + rhs.element[i][j];
            }
        }

        return result;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- () const {
        Matrix<T, R, C> result;
        
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.element[i][j] = -element[i][j];
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
                result.element[i][j] = element[i][j] - rhs.element[i][j];
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
                element[i][j] += rhs.element[i][j];
            }
        }
        
        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                element[i][j] -= rhs.element[i][j];
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
