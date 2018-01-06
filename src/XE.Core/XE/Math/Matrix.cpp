
#include "Matrix.hpp"
#include "Vector.hpp"

namespace XE::Math {
    template<typename T, int R, int C>
    Vector<T, C> Matrix<T, R, C>::RowVector(const int row) const {
        Vector<T, C> result;

        for (int col=0; col<C; col++) {
            result.Data[col] = Element[row][col];
        }

        return result;
    }

    template<typename T, int R, int C>
    Vector<T, R> Matrix<T, R, C>::ColumnVector(const int col) const {
        Vector<T, R> result;

        for (int row=0; row<R; row++) {
            result.Data[row] = Element[row][col];
        }

        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R - 1, C - 1> Matrix<T, R, C>::SubMatrix(const int row, const int column) const {
        Matrix<T, R - 1, C - 1> result;

        int ii = 0, jj = 0;

        for (int i=0; i<R; ++i) {
            if (i == row) {
                continue;
            }

            for(int j=0; j<C; ++j) {
                if (j == column) {
                    continue;
                }

                result.Element[ii][jj] = Element[i][j]; 
                ++jj;
            }
            
            ++ii;
            jj = 0;
        }
    
        return result;
    }

    template<typename T, int R, int C>
    T Matrix<T, R, C>::Determinant() const {
        static_assert(R == C);
        static_assert(R >= 2);

        constexpr if (R == 2 && C == 2) {
            return M22 * M11 - M12 * M21;
        } else {
            T factor = T(1);
            T result = T(0);
            
            const int row = 0;
                
            for (int column=0; column<Count; column++) {
                factor = (column + 1)%2 ? T(-1) : T(1);

                const T subDet = SubMatrix(row, column).Determinant();

                result += factor * Element[row][column] * subDet;
            }
                
            return result;
        }
    }

    template<typename T, int R, int C>
    Matrix<T, C, R> Matrix<T, R, C>::Transpose() const {
        Matrix<T, C, R> result;

        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.Element[j][i] = Element[i][j];
            }
        }

        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::Inverse() const {
        return Matrix<T, R, C>();
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::Inverse(const T determinant) const {
        return Matrix<T, R, C>();
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator+ (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;

        for (int i=0; i<R*C; i++) {
            result.Data[i] = Data[i] + rhs.Data[i];
        }

        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator- (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;

        for (int i=0; i<R*C; i++) {
            result.Data[i] = Data[i] - rhs.Data[i];
        }

        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator* (const Matrix<T, R, C>& rhs) const {
        Matrix<T, R, C> result;

        for (int i=0; i<R; i++) {
            for (int j=0; j<C; j++) {
                result.Element[i][j] = Dot(RowVector(i), rhs.ColumnVector(j));
            }
        }

        return result;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C> Matrix<T, R, C>::operator/ (const Matrix<T, R, C>& rhs) const {
        return *this * rhs.Inverse(rhs);
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator+= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            Data[i] += rhs.Data[i];
        }

        return *this;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator-= (const Matrix<T, R, C>& rhs) {
        for (int i=0; i<R*C; i++) {
            Data[i] -= rhs.Data[i];
        }

        return *this;
    }
    
    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator*= (const Matrix<T, R, C>& rhs) {
        return *this;
    }

    template<typename T, int R, int C>
    Matrix<T, R, C>& Matrix<T, R, C>::operator/= (const Matrix<T, R, C>& rhs) {
        return *this;
    }
    
    template<> struct Matrix<float, 2, 2>;
    template<> struct Matrix<float, 3, 3>;
    template<> struct Matrix<float, 4, 4>;
}