/**
 * @file Matrix.h
 * @brief GLM-style matrix types for xe::math.
 *
 * @ref xe::tmat<T,C,R> is a @c C-column, @c R-row matrix stored in
 * column-major order so that @c data() yields the same bit pattern as
 * @c glm::mat<C, R, T, Q>::data(). The convenience aliases @c mat2,
 * @c mat3, @c mat4 (and the @c dmat2, @c dmat3, @c dmat4 double-precision
 * variants) mirror glm's public surface.
 *
 * Element access:
 *   - @c m[col]          — returns a reference to the @c col-th column as a @c tvec<T,R>
 *   - @c m[col][row]     — scalar access, identical to glm
 *   - @c m(row, col)     — row/column scalar access (legacy convenience, not present in glm)
 *
 * Construction helpers @ref translate, @ref rotate, @ref scale,
 * @ref lookAtRH, @ref lookAt, @ref perspective, @ref ortho match glm's
 * functions with the same arguments and sign conventions: right-handed
 * coordinate system, NDC depth in @c [-1, 1], column-vector convention so
 * that @c new_point @c = @c M @c * @c point. Because the in-memory layout
 * is glm-identical, a @ref mat4 can be uploaded to OpenGL with
 * @c transpose=false and behave exactly like the equivalent @c glm::mat4.
 */

#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <ostream>

#include "Common.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief @c C columns × @c R rows matrix in column-major storage.
     * Layout matches @c glm::mat<C, R, T> bit-for-bit so the same buffer
     * can be passed to OpenGL or any other glm-aware API without
     * transposition. Storage is held as an array of column vectors, which
     * makes column accesses (@c m[c]) free and row accesses (@c getRow)
     * walk @c C separate columns.
     */
    template <typename T, int C, int R> struct tmat {
        /**
         * @brief Column-major storage: @c cols[c] is the @c c-th column as an @c R-dimensional vector.
         */
        tvec<T, R> cols[C];

        /**
         * @brief Default-construct to the zero matrix.
         * Note: this is the @c glm::mat4(0) default — call @c tmat(T(1)) to
         * obtain the identity matrix.
         */
        constexpr tmat() noexcept = default;

        /**
         * @brief Diagonal scalar constructor — produces @c s * identity.
         * Matches @c glm::mat4(1.0f), so @c tmat<float,4,4>(1.0f) yields
         * the identity matrix. Off-diagonal entries are zero.
         * @param s Scalar placed on the diagonal.
         */
        constexpr explicit tmat(T s) noexcept {
            for (int c = 0; c < C; ++c) {
                for (int r = 0; r < R; ++r) {
                    cols[c][r] = (c == r) ? s : T{};
                }
            }
        }

        /**
         * @brief Construct from a raw pointer to @c C*R values in column-major order.
         * The caller must ensure @p values points to at least @c C*R readable
         * scalars. The element at index @c c*R + r becomes @c cols[c][r], so
         * the buffer can be the @c data() of any glm or xe matrix of the
         * same dimensions.
         * @param values Pointer to a column-major buffer of length @c C*R.
         */
        constexpr explicit tmat(const T *values) noexcept {
            for (int c = 0; c < C; ++c) {
                for (int r = 0; r < R; ++r) {
                    cols[c][r] = values[c * R + r];
                }
            }
        }

        /**
         * @brief Construct from an initializer list of @c C*R values in column-major order.
         * Convenient for literal-driven test fixtures: callers can write
         * @c tmat<float,2,2>{1,2,3,4} and read components left-to-right
         * column-by-column.
         * @param il Initializer list of exactly @c C*R scalar values.
         */
        constexpr tmat(std::initializer_list<T> il) noexcept {
            int i = 0;
            for (const T value : il) {
                cols[i / R][i % R] = value;
                ++i;
            }
        }

        /**
         * @brief Construct from an array of column vectors.
         * Each element of @p columns becomes one column of the resulting
         * matrix. Often clearer than the flat-list constructor when the
         * caller already has the columns assembled.
         * @param columns Array of @c C column vectors of length @c R each.
         */
        constexpr explicit tmat(const std::array<tvec<T, R>, C> &columns) noexcept {
            for (int c = 0; c < C; ++c) {
                cols[c] = columns[c];
            }
        }

        /**
         * @brief glm-style column access: @c m[c] returns the @c c-th column.
         * @param c Column index in @c [0, C).
         * @return Mutable reference to the @c c-th column as an @c R-dimensional vector.
         */
        [[nodiscard]] constexpr tvec<T, R> &operator[](int c) noexcept {
            return cols[c];
        }

        /**
         * @brief glm-style const column access.
         * @param c Column index in @c [0, C).
         * @return Const reference to the @c c-th column as an @c R-dimensional vector.
         */
        [[nodiscard]] constexpr const tvec<T, R> &operator[](int c) const noexcept {
            return cols[c];
        }

        /**
         * @brief Row/column scalar access — legacy convenience, not present in glm.
         * Translates a logical @c (row, col) pair into the column-major
         * storage. Useful when porting code that thinks in row-major terms
         * or when implementing classical linear-algebra formulae that read
         * naturally as @c m(i, j).
         * @param row Row index in @c [0, R).
         * @param col Column index in @c [0, C).
         * @return Mutable reference to the scalar at the given row and column.
         */
        [[nodiscard]] constexpr T &operator()(int row, int col) noexcept {
            return cols[col][row];
        }

        /**
         * @brief Const row/column scalar access — legacy convenience.
         * @param row Row index in @c [0, R).
         * @param col Column index in @c [0, C).
         * @return Const reference to the scalar at the given row and column.
         */
        [[nodiscard]] constexpr const T &operator()(int row, int col) const noexcept {
            return cols[col][row];
        }

        /**
         * @brief Mutable pointer to the contiguous, column-major scalar storage.
         * Suitable for direct upload to OpenGL via
         * @c glUniformMatrix4fv(loc, 1, GL_FALSE, data()).
         * @return Pointer to @c &cols[0][0]; valid for @c C*R scalars.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return cols[0].data();
        }

        /**
         * @brief Const pointer to the contiguous, column-major scalar storage.
         * @return Const pointer to @c &cols[0][0]; valid for @c C*R scalars.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return cols[0].data();
        }

        /**
         * @brief Return a copy of the @c c-th column.
         * Equivalent to @c (*this)[c] but expresses intent more clearly when
         * a value (not a reference) is desired.
         * @param c Column index in @c [0, C).
         * @return Copy of the @c c-th column.
         */
        [[nodiscard]] constexpr tvec<T, R> getColumn(int c) const noexcept {
            return cols[c];
        }

        /**
         * @brief Return a copy of the @c r-th row, gathered across all @c C columns.
         * Walks the columns one by one, so it is @c O(C); prefer
         * @ref getColumn for tight loops.
         * @param r Row index in @c [0, R).
         * @return The @c r-th row as a @c C-dimensional vector.
         */
        [[nodiscard]] constexpr tvec<T, C> getRow(int r) const noexcept {
            tvec<T, C> result;
            for (int c = 0; c < C; ++c) {
                result[c] = cols[c][r];
            }
            return result;
        }

        /**
         * @brief Replace the @c c-th column with @p v.
         * @param c Column index in @c [0, C).
         * @param v New column contents.
         * @return Reference to @c *this for fluent chaining.
         */
        constexpr tmat &setColumn(int c, const tvec<T, R> &v) noexcept {
            cols[c] = v;
            return *this;
        }

        /**
         * @brief Replace the @c r-th row with @p v.
         * Each component of @p v is written into the matching column.
         * @param r Row index in @c [0, R).
         * @param v New row contents.
         * @return Reference to @c *this for fluent chaining.
         */
        constexpr tmat &setRow(int r, const tvec<T, C> &v) noexcept {
            for (int c = 0; c < C; ++c) {
                cols[c][r] = v[c];
            }
            return *this;
        }

        /**
         * @brief Return the @c (R-1)x(C-1) minor obtained by removing one row and one column.
         * Used by @ref determinant and @ref adjoint to perform cofactor
         * expansion. Indices outside the valid range are not validated.
         * @param skipRow Row to remove, in @c [0, R).
         * @param skipCol Column to remove, in @c [0, C).
         * @return The reduced matrix with one fewer row and one fewer column.
         */
        [[nodiscard]] constexpr tmat<T, C - 1, R - 1> getSubMatrix(int skipRow, int skipCol) const noexcept {
            tmat<T, C - 1, R - 1> result;
            int dstRow = 0;
            for (int r = 0; r < R; ++r) {
                if (r == skipRow) {
                    continue;
                }
                int dstCol = 0;
                for (int c = 0; c < C; ++c) {
                    if (c == skipCol) {
                        continue;
                    }
                    result(dstRow, dstCol) = (*this)(r, c);
                    ++dstCol;
                }
                ++dstRow;
            }
            return result;
        }
    };

    // ---------------------------------------------------------------------
    // Operators.
    // ---------------------------------------------------------------------

    /**
     * @brief Strict equality comparison between two matrices of the same dimensions.
     * Performs an exact per-component comparison; for floating-point matrices
     * an epsilon-aware comparison is usually a better idea.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when every column of @p a equals the matching column of @p b.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr bool operator==(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            if (a[c] != b[c]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Strict inequality comparison.
     * @param a Left operand.
     * @param b Right operand.
     * @return @c true when at least one column differs.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr bool operator!=(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        return !(a == b);
    }

    /**
     * @brief Unary plus — returns a copy of @p m unchanged.
     * Provided so generic code can apply a sign without dispatching.
     * @param m Matrix to copy.
     * @return A copy of @p m.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator+(const tmat<T, C, R> &m) noexcept {
        return m;
    }

    /**
     * @brief Componentwise negation.
     * @param m Matrix to negate.
     * @return A matrix whose columns are the negation of @p m's columns.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator-(const tmat<T, C, R> &m) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = -m[c];
        }
        return r;
    }

    /**
     * @brief Componentwise matrix addition.
     * @param a Left operand.
     * @param b Right operand.
     * @return The matrix whose columns are @c a[c] + b[c].
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator+(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = a[c] + b[c];
        }
        return r;
    }

    /**
     * @brief Componentwise matrix subtraction.
     * @param a Left operand.
     * @param b Right operand.
     * @return The matrix whose columns are @c a[c] - b[c].
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator-(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = a[c] - b[c];
        }
        return r;
    }

    /**
     * @brief Matrix-by-scalar multiplication.
     * @param m Matrix operand.
     * @param s Scalar operand.
     * @return The matrix whose columns are @c m[c] * s.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator*(const tmat<T, C, R> &m, T s) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = m[c] * s;
        }
        return r;
    }

    /**
     * @brief Scalar-by-matrix multiplication (commutative form).
     * @param s Scalar operand.
     * @param m Matrix operand.
     * @return The matrix whose columns are @c m[c] * s.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator*(T s, const tmat<T, C, R> &m) noexcept {
        return m * s;
    }

    /**
     * @brief Matrix-by-scalar division.
     * @param m Matrix operand.
     * @param s Scalar divisor; behaviour is undefined when zero.
     * @return The matrix whose columns are @c m[c] / s.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator/(const tmat<T, C, R> &m, T s) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = m[c] / s;
        }
        return r;
    }

    /**
     * @brief In-place componentwise matrix addition.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int C, int R> constexpr tmat<T, C, R> &operator+=(tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            a[c] += b[c];
        }
        return a;
    }

    /**
     * @brief In-place componentwise matrix subtraction.
     * @param a Left operand, modified in place.
     * @param b Right operand.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int C, int R> constexpr tmat<T, C, R> &operator-=(tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            a[c] -= b[c];
        }
        return a;
    }

    /**
     * @brief Matrix times column vector — @c mat<C,R> @c * @c vec<C> @c = @c vec<R>.
     * Standard column-vector convention used by glm and OpenGL: the vector
     * is treated as a column on the right of the matrix, and each row of
     * the result is the dot product of the corresponding matrix row with
     * the vector.
     * @param m Left operand (transformation matrix).
     * @param v Right operand (column vector).
     * @return The transformed vector of length @c R.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tvec<T, R> operator*(const tmat<T, C, R> &m, const tvec<T, C> &v) noexcept {
        tvec<T, R> result;
        for (int r = 0; r < R; ++r) {
            T acc = T{};
            for (int c = 0; c < C; ++c) {
                acc += m(r, c) * v[c];
            }
            result[r] = acc;
        }
        return result;
    }

    /**
     * @brief Row vector times matrix — @c vec<R> @c * @c mat<C,R> @c = @c vec<C>.
     * The mirror of @ref operator*(const tmat<T,C,R>&, const tvec<T,C>&).
     * Each component of the result is the dot product of @p v with the
     * corresponding column of @p m.
     * @param v Left operand (row vector).
     * @param m Right operand (transformation matrix).
     * @return The transformed vector of length @c C.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tvec<T, C> operator*(const tvec<T, R> &v, const tmat<T, C, R> &m) noexcept {
        tvec<T, C> result;
        for (int c = 0; c < C; ++c) {
            T acc = T{};
            for (int r = 0; r < R; ++r) {
                acc += v[r] * m(r, c);
            }
            result[c] = acc;
        }
        return result;
    }

    /**
     * @brief Matrix times matrix — @c mat<T,Ca,Ra> @c * @c mat<T,Cb,Ca> @c = @c mat<T,Cb,Ra>.
     * Inner dimensions @c Ca must match. Composition follows the
     * post-multiply convention: @c (A*B)*v == A*(B*v), i.e. @p b is applied
     * to a vector first, then @p a.
     * @param a Left operand.
     * @param b Right operand whose row count equals @p a's column count.
     * @return The product matrix with @c Cb columns and @c Ra rows.
     */
    template <typename T, int Ca, int Ra, int Cb> [[nodiscard]] constexpr tmat<T, Cb, Ra> operator*(const tmat<T, Ca, Ra> &a, const tmat<T, Cb, Ca> &b) noexcept {
        tmat<T, Cb, Ra> result;
        for (int r = 0; r < Ra; ++r) {
            for (int c = 0; c < Cb; ++c) {
                T acc = T{};
                for (int k = 0; k < Ca; ++k) {
                    acc += a(r, k) * b(k, c);
                }
                result(r, c) = acc;
            }
        }
        return result;
    }

    /**
     * @brief In-place square-matrix multiplication.
     * Defined only for square matrices because the dimensions of the
     * product must match the dimensions of @p a.
     * @param a Left operand, modified in place.
     * @param b Right operand of the same dimensions.
     * @return Reference to @p a after the assignment.
     */
    template <typename T, int N> constexpr tmat<T, N, N> &operator*=(tmat<T, N, N> &a, const tmat<T, N, N> &b) noexcept {
        a = a * b;
        return a;
    }

    // ---------------------------------------------------------------------
    // Linear algebra functions (names match glm).
    // ---------------------------------------------------------------------

    /**
     * @brief Return the transpose of @p m.
     * Swaps the matrix's row and column counts: a @c CxR matrix becomes
     * @c RxC. Equivalent to @c glm::transpose.
     * @param m Input matrix.
     * @return The transposed matrix.
     */
    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, R, C> transpose(const tmat<T, C, R> &m) noexcept {
        tmat<T, R, C> result;
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                result(c, r) = m(r, c);
            }
        }
        return result;
    }

    /**
     * @brief Compute the determinant of a square matrix.
     * Uses recursive cofactor expansion along the first row. The 1x1 and
     * 2x2 cases are special-cased for efficiency; larger matrices fall back
     * to recursion. Equivalent to @c glm::determinant.
     * @param m Square matrix.
     * @return The scalar determinant.
     */
    template <typename T, int N> [[nodiscard]] constexpr T determinant(const tmat<T, N, N> &m) noexcept {
        if constexpr (N == 1) {
            return m(0, 0);
        } else if constexpr (N == 2) {
            return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
        } else {
            T result = T{};
            for (int c = 0; c < N; ++c) {
                const T factor = (c % 2 == 0) ? T(1) : T(-1);
                result += factor * m(0, c) * determinant(m.getSubMatrix(0, c));
            }
            return result;
        }
    }

    /**
     * @brief Compute the adjugate (classical adjoint) of a square matrix.
     * Each entry @c (i,j) of the result is the @c (j,i)-cofactor of @p m.
     * Used internally by @ref inverse but exposed for callers that need
     * the unscaled cofactor matrix directly.
     * @param m Square matrix.
     * @return The adjugate matrix.
     */
    template <typename T, int N> [[nodiscard]] constexpr tmat<T, N, N> adjoint(const tmat<T, N, N> &m) noexcept {
        tmat<T, N, N> cofactors;
        if constexpr (N == 2) {
            cofactors(0, 0) = m(1, 1);
            cofactors(0, 1) = -m(1, 0);
            cofactors(1, 0) = -m(0, 1);
            cofactors(1, 1) = m(0, 0);
            return cofactors;
        } else {
            for (int r = 0; r < N; ++r) {
                for (int c = 0; c < N; ++c) {
                    const T factor = ((r + c) % 2 == 0) ? T(1) : T(-1);
                    cofactors(c, r) = factor * determinant(m.getSubMatrix(r, c));
                }
            }
            return cofactors;
        }
    }

    /**
     * @brief Return the inverse of a square matrix.
     * Computes @c adjoint(m) @c / @c determinant(m). The result is
     * undefined when @p m is singular (determinant is zero); callers that
     * already have the determinant should prefer the two-argument overload
     * to avoid recomputing it.
     * @param m Square matrix; must be non-singular.
     * @return The inverse matrix.
     */
    template <typename T, int N> [[nodiscard]] constexpr tmat<T, N, N> inverse(const tmat<T, N, N> &m) noexcept {
        return adjoint(m) / determinant(m);
    }

    /**
     * @brief Return the inverse of a square matrix, given a precomputed determinant.
     * Skips the redundant @ref determinant call when the caller already
     * computed it (for example, while testing for singularity).
     * @param m Square matrix; must be non-singular.
     * @param det Precomputed determinant of @p m; must be non-zero.
     * @return The inverse matrix.
     */
    template <typename T, int N> [[nodiscard]] constexpr tmat<T, N, N> inverse(const tmat<T, N, N> &m, T det) noexcept {
        return adjoint(m) / det;
    }

    // ---------------------------------------------------------------------
    // Transformation builders (glm-compatible).
    //
    // These produce matrices in the usual "right-handed, column-vector"
    // convention, meaning the resulting matrices can be multiplied on the
    // left of a point as new_point = M * point and, when uploaded to
    // OpenGL with transpose=false, behave identically to glm.
    // ---------------------------------------------------------------------

    /**
     * @brief Apply a non-uniform scale to an existing matrix.
     * Equivalent to @c glm::scale(m, v); the columns of @p m are scaled by
     * the matching components of @p v while the translation column is left
     * untouched.
     * @param m Matrix to scale.
     * @param v Per-axis scale factors.
     * @return The scaled matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> scale(const tmat<T, 4, 4> &m, const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> result;
        result[0] = m[0] * v.x;
        result[1] = m[1] * v.y;
        result[2] = m[2] * v.z;
        result[3] = m[3];
        return result;
    }

    /**
     * @brief Build a non-uniform scale matrix from a 3-vector.
     * The result has @c v.x, @c v.y, @c v.z on the diagonal and @c 1 in
     * the @c (3,3) corner — multiplying it by a column vector scales each
     * component independently.
     * @param v Per-axis scale factors.
     * @return The 4×4 scale matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> scale(const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 0) = v.x;
        m(1, 1) = v.y;
        m(2, 2) = v.z;
        return m;
    }

    /**
     * @brief Apply a translation to an existing matrix.
     * Equivalent to @c glm::translate(m, v): the translation column of the
     * result is @c m[0]*v.x @c + @c m[1]*v.y @c + @c m[2]*v.z @c + @c m[3],
     * which composes correctly with any prior rotation/scale stored in @p m.
     * @param m Matrix to translate.
     * @param v Translation vector.
     * @return The translated matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> translate(const tmat<T, 4, 4> &m, const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> result = m;
        result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
        return result;
    }

    /**
     * @brief Build a pure translation matrix.
     * Returns the identity with @p v written into the translation column.
     * @param v Translation vector.
     * @return The 4×4 translation matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> translate(const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 3) = v.x;
        m(1, 3) = v.y;
        m(2, 3) = v.z;
        return m;
    }

    /**
     * @brief Right-handed rotation about the X axis.
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotateX(T radians) noexcept {
        tmat<T, 4, 4> m(T(1));
        const T c = std::cos(radians);
        const T s = std::sin(radians);
        m(1, 1) = c;
        m(1, 2) = -s;
        m(2, 1) = s;
        m(2, 2) = c;
        return m;
    }

    /**
     * @brief Right-handed rotation about the Y axis.
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotateY(T radians) noexcept {
        tmat<T, 4, 4> m(T(1));
        const T c = std::cos(radians);
        const T s = std::sin(radians);
        m(0, 0) = c;
        m(0, 2) = s;
        m(2, 0) = -s;
        m(2, 2) = c;
        return m;
    }

    /**
     * @brief Right-handed rotation about the Z axis.
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotateZ(T radians) noexcept {
        tmat<T, 4, 4> m(T(1));
        const T c = std::cos(radians);
        const T s = std::sin(radians);
        m(0, 0) = c;
        m(0, 1) = -s;
        m(1, 0) = s;
        m(1, 1) = c;
        return m;
    }

    /**
     * @brief Build an arbitrary-axis rotation matrix.
     * Implements Rodrigues' rotation formula and returns the same value as
     * @c glm::rotate(mat4(1), angle, axis). The axis does not need to be
     * normalised — it is normalised internally.
     * @param radians Angle in radians.
     * @param axis Rotation axis; non-zero, normalised internally.
     * @return The 4×4 rotation matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotate(T radians, const tvec<T, 3> &axis) {
        const T c = std::cos(radians);
        const T s = std::sin(radians);
        const tvec<T, 3> n = normalize(axis);
        const T one_minus_c = T(1) - c;

        tmat<T, 4, 4> m(T(1));

        m(0, 0) = c + n.x * n.x * one_minus_c;
        m(0, 1) = n.x * n.y * one_minus_c - n.z * s;
        m(0, 2) = n.x * n.z * one_minus_c + n.y * s;

        m(1, 0) = n.y * n.x * one_minus_c + n.z * s;
        m(1, 1) = c + n.y * n.y * one_minus_c;
        m(1, 2) = n.y * n.z * one_minus_c - n.x * s;

        m(2, 0) = n.z * n.x * one_minus_c - n.y * s;
        m(2, 1) = n.z * n.y * one_minus_c + n.x * s;
        m(2, 2) = c + n.z * n.z * one_minus_c;

        return m;
    }

    /**
     * @brief Apply an arbitrary-axis rotation to an existing matrix.
     * Equivalent to @c glm::rotate(m, angle, axis): post-multiplies @p m
     * by the rotation matrix produced by the single-argument @ref rotate
     * overload.
     * @param m Matrix to rotate.
     * @param radians Angle in radians.
     * @param axis Rotation axis; normalised internally.
     * @return The rotated matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotate(const tmat<T, 4, 4> &m, T radians, const tvec<T, 3> &axis) {
        return m * rotate(radians, axis);
    }

    /**
     * @brief Build a right-handed view matrix that looks from @p eye toward @p center.
     * Matches @c glm::lookAtRH; the resulting matrix orients the camera so
     * that the @c +Y axis of the rotated frame points along @p up
     * (re-orthogonalised) and @c -Z points at @p center.
     * @param eye Camera position in world space.
     * @param center Point the camera looks at.
     * @param up World-space up direction; need not be exactly orthogonal to the view direction.
     * @return The 4×4 view matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> lookAtRH(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        const tvec<T, 3> f = normalize(center - eye);
        const tvec<T, 3> s = normalize(cross(f, up));
        const tvec<T, 3> u = cross(s, f);

        tmat<T, 4, 4> m(T(1));
        m(0, 0) = s.x;
        m(0, 1) = s.y;
        m(0, 2) = s.z;
        m(1, 0) = u.x;
        m(1, 1) = u.y;
        m(1, 2) = u.z;
        m(2, 0) = -f.x;
        m(2, 1) = -f.y;
        m(2, 2) = -f.z;
        m(0, 3) = -dot(s, eye);
        m(1, 3) = -dot(u, eye);
        m(2, 3) = dot(f, eye);
        return m;
    }

    /**
     * @brief Default-handed @c lookAt — forwards to @ref lookAtRH.
     * Provided for symmetry with the rest of the engine, which assumes
     * right-handed coordinates throughout.
     * @param eye Camera position.
     * @param center Look-at point.
     * @param up World-space up direction.
     * @return The 4×4 view matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> lookAt(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH(eye, center, up);
    }

    /**
     * @brief Build a right-handed perspective projection matrix.
     * Matches @c glm::perspectiveRH with NDC depth in @c [-1, 1] (the
     * OpenGL default). All four arguments are validated with @c assert in
     * debug builds.
     * @param fovyRadians Vertical field-of-view in radians; must be > 0.
     * @param aspect Width/height aspect ratio; must be > 0.
     * @param znear Distance to the near plane; must be > 0.
     * @param zfar Distance to the far plane; must be > @p znear.
     * @return The 4×4 projection matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> perspectiveRH(T fovyRadians, T aspect, T znear, T zfar) {
        assert(fovyRadians > T(0));
        assert(aspect > T(0));
        assert(znear > T(0));
        assert(zfar > znear);

        const T tan_half = std::tan(fovyRadians / T(2));

        tmat<T, 4, 4> m;
        m(0, 0) = T(1) / (aspect * tan_half);
        m(1, 1) = T(1) / tan_half;
        m(2, 2) = -(zfar + znear) / (zfar - znear);
        m(3, 2) = -T(1);
        m(2, 3) = -(T(2) * zfar * znear) / (zfar - znear);
        return m;
    }

    /**
     * @brief Default-handed perspective projection — forwards to @ref perspectiveRH.
     * @param fovyRadians Vertical field-of-view in radians.
     * @param aspect Width/height aspect ratio.
     * @param znear Distance to the near plane.
     * @param zfar Distance to the far plane.
     * @return The 4×4 projection matrix.
     */
    template <typename T> [[nodiscard]] tmat<T, 4, 4> perspective(T fovyRadians, T aspect, T znear, T zfar) {
        return perspectiveRH(fovyRadians, aspect, znear, zfar);
    }

    /**
     * @brief Build a right-handed orthographic projection matrix.
     * NDC depth is mapped into @c [-1, 1], matching the OpenGL default.
     * Matches @c glm::orthoRH.
     * @param left Left clipping plane in view space.
     * @param right Right clipping plane in view space.
     * @param bottom Bottom clipping plane in view space.
     * @param top Top clipping plane in view space.
     * @param znear Near clipping plane.
     * @param zfar Far clipping plane.
     * @return The 4×4 orthographic projection matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> orthoRH(T left, T right, T bottom, T top, T znear, T zfar) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 0) = T(2) / (right - left);
        m(1, 1) = T(2) / (top - bottom);
        m(2, 2) = -T(2) / (zfar - znear);
        m(0, 3) = -(right + left) / (right - left);
        m(1, 3) = -(top + bottom) / (top - bottom);
        m(2, 3) = -(zfar + znear) / (zfar - znear);
        return m;
    }

    /**
     * @brief Default-handed orthographic projection — forwards to @ref orthoRH.
     * @param left Left clipping plane.
     * @param right Right clipping plane.
     * @param bottom Bottom clipping plane.
     * @param top Top clipping plane.
     * @param znear Near clipping plane.
     * @param zfar Far clipping plane.
     * @return The 4×4 orthographic projection matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> ortho(T left, T right, T bottom, T top, T znear, T zfar) noexcept {
        return orthoRH(left, right, bottom, top, znear, zfar);
    }

    // ---------------------------------------------------------------------
    // Debug output.
    // ---------------------------------------------------------------------

    /**
     * @brief Stream insertion for debugging and test failure reports.
     * Prints the matrix one row at a time with fixed precision so the
     * output stays aligned regardless of magnitude. Not intended as a
     * serialisation format.
     * @param os Output stream.
     * @param m Matrix to print.
     * @return The same stream, to allow chaining.
     */
    template <typename T, int C, int R> inline std::ostream &operator<<(std::ostream &os, const tmat<T, C, R> &m) {
        os << "xe::tmat<" << C << ", " << R << "> {\n";
        for (int r = 0; r < R; ++r) {
            os << "  { ";
            for (int c = 0; c < C; ++c) {
                os << std::fixed << std::setprecision(6) << std::setw(12) << m(r, c);
                if (c < C - 1) {
                    os << ", ";
                }
            }
            os << " }";
            if (r < R - 1) {
                os << ",";
            }
            os << "\n";
        }
        os << "}";
        return os;
    }

    // ---------------------------------------------------------------------
    // Canonical type aliases.
    // ---------------------------------------------------------------------

    using mat2 = tmat<float, 2, 2>; ///< 2×2 single-precision matrix — glm-compatible alias.
    using mat3 = tmat<float, 3, 3>; ///< 3×3 single-precision matrix — glm-compatible alias.
    using mat4 = tmat<float, 4, 4>; ///< 4×4 single-precision matrix — glm-compatible alias.

    using dmat2 = tmat<double, 2, 2>; ///< 2×2 double-precision matrix.
    using dmat3 = tmat<double, 3, 3>; ///< 3×3 double-precision matrix.
    using dmat4 = tmat<double, 4, 4>; ///< 4×4 double-precision matrix.

    // ---------------------------------------------------------------------
    // Legacy PascalCase aliases. Old TMatrix<T, R, C> had R rows and C columns;
    // new tmat<T, C, R> is stored column-major but the argument order is
    // indistinguishable for square matrices (the only ones used).
    // ---------------------------------------------------------------------

    template <typename T, int R, int C> using TMatrix = tmat<T, C, R>; ///< @deprecated Use @ref tmat. Note the column-major argument order.
    using Matrix2 = mat2;                                              ///< @deprecated Use @ref mat2.
    using Matrix3 = mat3;                                              ///< @deprecated Use @ref mat3.
    using Matrix4 = mat4;                                              ///< @deprecated Use @ref mat4.

    // ---------------------------------------------------------------------
    // Legacy matrix-builder free functions. Kept here (rather than Legacy.h)
    // so that any header pulling in just Matrix.h gets them.
    // ---------------------------------------------------------------------

    /**
     * @brief Legacy: build an @c NxN identity matrix.
     * @deprecated Construct @c tmat<T,N,N>(T(1)) directly.
     * @return Identity matrix of size @p N × @p N.
     */
    template <typename T = float, int N> [[nodiscard]] constexpr tmat<T, N, N> matIdentity() noexcept {
        return tmat<T, N, N>(T(1));
    }

    /**
     * @brief Legacy: build a 2×2 identity matrix.
     * @deprecated Construct @c mat2(1.0f) directly.
     * @return The 2×2 identity matrix.
     */
    template <typename T = float> [[nodiscard]] constexpr mat2 mat2Identity() noexcept {
        return mat2(T(1));
    }

    /**
     * @brief Legacy: build a 3×3 identity matrix.
     * @deprecated Construct @c mat3(1.0f) directly.
     * @return The 3×3 identity matrix.
     */
    template <typename T = float> [[nodiscard]] constexpr mat3 mat3Identity() noexcept {
        return mat3(T(1));
    }

    /**
     * @brief Legacy: build a 4×4 identity matrix.
     * @deprecated Construct @c mat4(1.0f) directly.
     * @return The 4×4 identity matrix.
     */
    template <typename T = float> [[nodiscard]] constexpr mat4 mat4Identity() noexcept {
        return mat4(T(1));
    }

    /**
     * @brief Legacy: build a 3×3 scaling matrix.
     * @deprecated Use @ref scale on a @ref mat3 if needed; this is a holdover from the pre-glm API.
     * @param s Per-axis scale factors.
     * @return The 3×3 scale matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 3, 3> mat3Scaling(const tvec<T, 3> &s) noexcept {
        tmat<T, 3, 3> m(T(1));
        m(0, 0) = s.x;
        m(1, 1) = s.y;
        m(2, 2) = s.z;
        return m;
    }

    /**
     * @brief Legacy: build a 4×4 scaling matrix from a 4-vector (homogeneous components).
     * @deprecated Use @ref scale instead.
     * @param s Per-axis scale factors including the homogeneous @c w.
     * @return The 4×4 scale matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Scaling(const tvec<T, 4> &s) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 0) = s.x;
        m(1, 1) = s.y;
        m(2, 2) = s.z;
        m(3, 3) = s.w;
        return m;
    }

    /**
     * @brief Legacy: build a 4×4 scaling matrix from a 3-vector.
     * @deprecated Use @ref scale.
     * @see scale
     * @param s Per-axis scale factors.
     * @return The 4×4 scale matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Scaling(const tvec<T, 3> &s) noexcept {
        return scale(s);
    }

    /**
     * @brief Legacy: build a 3×3 translation matrix for 2D points.
     * @deprecated Use @ref translate after promoting to a 4×4 matrix.
     * @param t Translation in the @c (x, y) plane.
     * @return The 3×3 translation matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 3, 3> mat3Translation(const tvec<T, 2> &t) noexcept {
        tmat<T, 3, 3> m(T(1));
        m(0, 2) = t.x;
        m(1, 2) = t.y;
        return m;
    }

    /**
     * @brief Legacy: build a 4×4 translation matrix.
     * @deprecated Use @ref translate.
     * @see translate
     * @param t Translation vector.
     * @return The 4×4 translation matrix.
     */
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Translation(const tvec<T, 3> &t) noexcept {
        return translate(t);
    }

    /**
     * @brief Legacy: build a 3×3 X-axis rotation by extracting the upper-left of @ref rotateX.
     * @deprecated Use @ref rotateX or build directly with @ref tmat.
     * @param radians Angle in radians.
     * @return The 3×3 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 3, 3> mat3RotationX(T radians) {
        const auto m4 = rotateX<T>(radians);
        tmat<T, 3, 3> r(T(1));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r(i, j) = m4(i, j);
            }
        }
        return r;
    }

    /**
     * @brief Legacy: build a 3×3 Y-axis rotation.
     * @deprecated Use @ref rotateY.
     * @param radians Angle in radians.
     * @return The 3×3 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 3, 3> mat3RotationY(T radians) {
        const auto m4 = rotateY<T>(radians);
        tmat<T, 3, 3> r(T(1));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r(i, j) = m4(i, j);
            }
        }
        return r;
    }

    /**
     * @brief Legacy: build a 3×3 Z-axis rotation.
     * @deprecated Use @ref rotateZ.
     * @param radians Angle in radians.
     * @return The 3×3 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 3, 3> mat3RotationZ(T radians) {
        const auto m4 = rotateZ<T>(radians);
        tmat<T, 3, 3> r(T(1));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r(i, j) = m4(i, j);
            }
        }
        return r;
    }

    /**
     * @brief Legacy: alias for @ref rotateX.
     * @deprecated Use @ref rotateX.
     * @see rotateX
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationX(T radians) {
        return rotateX<T>(radians);
    }

    /**
     * @brief Legacy: alias for @ref rotateY.
     * @deprecated Use @ref rotateY.
     * @see rotateY
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationY(T radians) {
        return rotateY<T>(radians);
    }

    /**
     * @brief Legacy: alias for @ref rotateZ.
     * @deprecated Use @ref rotateZ.
     * @see rotateZ
     * @param radians Angle in radians.
     * @return The 4×4 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationZ(T radians) {
        return rotateZ<T>(radians);
    }

    /**
     * @brief Legacy: build a 2×2 rotation matrix for 2D points.
     * @deprecated Construct directly or use a complex number / @ref tquat.
     * @param radians Angle in radians.
     * @return The 2×2 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 2, 2> mat2Rotation(T radians) {
        tmat<T, 2, 2> m(T(1));
        const T c = std::cos(radians);
        const T s = std::sin(radians);
        m(0, 0) = c;
        m(0, 1) = -s;
        m(1, 0) = s;
        m(1, 1) = c;
        return m;
    }

    /**
     * @brief Legacy: 3×3 arbitrary-axis rotation extracted from the upper-left of @ref rotate.
     * @deprecated Use @ref rotate.
     * @param radians Angle in radians.
     * @param axis Rotation axis; normalised internally.
     * @return The 3×3 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 3, 3> mat3Rotation(T radians, const tvec<T, 3> &axis) {
        const auto m4 = rotate<T>(radians, axis);
        tmat<T, 3, 3> r(T(1));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r(i, j) = m4(i, j);
            }
        }
        return r;
    }

    /**
     * @brief Legacy: alias for @ref rotate.
     * @deprecated Use @ref rotate.
     * @see rotate
     * @param radians Angle in radians.
     * @param axis Rotation axis; normalised internally.
     * @return The 4×4 rotation matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Rotation(T radians, const tvec<T, 3> &axis) {
        return rotate<T>(radians, axis);
    }

    /**
     * @brief Legacy: alias for @ref lookAtRH.
     * @deprecated Use @ref lookAtRH.
     * @see lookAtRH
     * @param eye Camera position.
     * @param center Look-at point.
     * @param up Up direction.
     * @return The 4×4 view matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4LookAtRH(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH<T>(eye, center, up);
    }

    /**
     * @brief Legacy: default-handed @c lookAt.
     * @deprecated Use @ref lookAt.
     * @see lookAt
     * @param eye Camera position.
     * @param center Look-at point.
     * @param up Up direction.
     * @return The 4×4 view matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4LookAt(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH<T>(eye, center, up);
    }

    /**
     * @brief Legacy: alias for @ref perspectiveRH.
     * @deprecated Use @ref perspective or @ref perspectiveRH directly.
     * @see perspectiveRH
     * @param fovRadians Vertical field-of-view in radians.
     * @param aspect Width/height aspect ratio.
     * @param znear Near plane distance.
     * @param zfar Far plane distance.
     * @return The 4×4 perspective projection matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Perspective(T fovRadians, T aspect, T znear, T zfar) {
        return perspectiveRH<T>(fovRadians, aspect, znear, zfar);
    }

    /**
     * @brief Legacy: build a 4×4 orthographic projection from min/max corner points.
     * @deprecated Use @ref ortho or @ref orthoRH directly with explicit bounds.
     * @see orthoRH
     * @param pmin Minimum corner of the view-space box.
     * @param pmax Maximum corner of the view-space box.
     * @return The 4×4 orthographic projection matrix.
     */
    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Ortho(const tvec<T, 3> &pmin, const tvec<T, 3> &pmax) {
        return orthoRH<T>(pmin.x, pmax.x, pmin.y, pmax.y, pmin.z, pmax.z);
    }
} // namespace xe
