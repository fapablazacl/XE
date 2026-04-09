/**
 * @file Matrix.h
 * @brief GLM-style matrix types for xe::math.
 *
 * xe::tmat<T, C, R> is a C-column, R-row matrix stored in column-major order
 * so that @c data() yields the same bit pattern as glm::mat<C, R, T, Q>::data().
 * The convenience aliases @c mat2, @c mat3, @c mat4 (and the @c tmat2, @c tmat3,
 * @c tmat4 template ones) mirror glm's public surface.
 *
 * Element access:
 *   - @c m[col]          — returns a reference to the C-th column as a @c tvec<T,R>
 *   - @c m[col][row]     — scalar access, same as glm
 *   - @c m(row, col)     — row/column scalar access (legacy convenience, not in glm)
 *
 * Construction helpers @c translate, @c rotate, @c scale, @c lookAtRH, @c lookAt,
 * @c perspective, @c ortho match glm's functions with the same arguments and
 * sign conventions.
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
     * @brief C columns x R rows matrix, column-major storage (matches glm).
     */
    template <typename T, int C, int R> struct tmat {
        //! Column-major storage: cols[c] is the c-th column as an R-dimensional vector.
        tvec<T, R> cols[C];

        //! Zero matrix.
        constexpr tmat() noexcept = default;

        //! Diagonal scalar — produces an identity scaled by @p s. Matches glm::mat4(1.0f).
        constexpr explicit tmat(T s) noexcept {
            for (int c = 0; c < C; ++c) {
                for (int r = 0; r < R; ++r) {
                    cols[c][r] = (c == r) ? s : T{};
                }
            }
        }

        //! From a raw pointer to C*R values in column-major order.
        constexpr explicit tmat(const T *values) noexcept {
            for (int c = 0; c < C; ++c) {
                for (int r = 0; r < R; ++r) {
                    cols[c][r] = values[c * R + r];
                }
            }
        }

        //! From an initializer list of C*R values in column-major order.
        constexpr tmat(std::initializer_list<T> il) noexcept {
            int i = 0;
            for (const T value : il) {
                cols[i / R][i % R] = value;
                ++i;
            }
        }

        //! From an array of columns.
        constexpr explicit tmat(const std::array<tvec<T, R>, C> &columns) noexcept {
            for (int c = 0; c < C; ++c) {
                cols[c] = columns[c];
            }
        }

        //! Column access (glm style): m[col] returns the C-th column.
        [[nodiscard]] constexpr tvec<T, R> &operator[](int c) noexcept {
            return cols[c];
        }

        [[nodiscard]] constexpr const tvec<T, R> &operator[](int c) const noexcept {
            return cols[c];
        }

        //! Row/col scalar access (legacy convenience — not present in glm).
        [[nodiscard]] constexpr T &operator()(int row, int col) noexcept {
            return cols[col][row];
        }

        [[nodiscard]] constexpr const T &operator()(int row, int col) const noexcept {
            return cols[col][row];
        }

        [[nodiscard]] constexpr T *data() noexcept {
            return cols[0].data();
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return cols[0].data();
        }

        //! Returns a copy of the c-th column.
        [[nodiscard]] constexpr tvec<T, R> getColumn(int c) const noexcept {
            return cols[c];
        }

        //! Returns a copy of the r-th row.
        [[nodiscard]] constexpr tvec<T, C> getRow(int r) const noexcept {
            tvec<T, C> result;
            for (int c = 0; c < C; ++c) {
                result[c] = cols[c][r];
            }
            return result;
        }

        constexpr tmat &setColumn(int c, const tvec<T, R> &v) noexcept {
            cols[c] = v;
            return *this;
        }

        constexpr tmat &setRow(int r, const tvec<T, C> &v) noexcept {
            for (int c = 0; c < C; ++c) {
                cols[c][r] = v[c];
            }
            return *this;
        }

        //! Returns the (R-1)x(C-1) minor obtained by removing @p skipRow and @p skipCol.
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

    template <typename T, int C, int R> [[nodiscard]] constexpr bool operator==(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            if (a[c] != b[c]) {
                return false;
            }
        }
        return true;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr bool operator!=(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        return !(a == b);
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator+(const tmat<T, C, R> &m) noexcept {
        return m;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator-(const tmat<T, C, R> &m) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = -m[c];
        }
        return r;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator+(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = a[c] + b[c];
        }
        return r;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator-(const tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = a[c] - b[c];
        }
        return r;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator*(const tmat<T, C, R> &m, T s) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = m[c] * s;
        }
        return r;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator*(T s, const tmat<T, C, R> &m) noexcept {
        return m * s;
    }

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, C, R> operator/(const tmat<T, C, R> &m, T s) noexcept {
        tmat<T, C, R> r;
        for (int c = 0; c < C; ++c) {
            r[c] = m[c] / s;
        }
        return r;
    }

    template <typename T, int C, int R> constexpr tmat<T, C, R> &operator+=(tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            a[c] += b[c];
        }
        return a;
    }

    template <typename T, int C, int R> constexpr tmat<T, C, R> &operator-=(tmat<T, C, R> &a, const tmat<T, C, R> &b) noexcept {
        for (int c = 0; c < C; ++c) {
            a[c] -= b[c];
        }
        return a;
    }

    //! Matrix * column vector: mat<C,R> * vec<C> = vec<R>. Same convention as glm.
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

    //! row vector * matrix: vec<R> * mat<C,R> = vec<C>.
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

    //! Matrix * matrix: mat<T,Ca,Ra> * mat<T,Cb,Cb==Ra> = mat<T,Cb,Ra>.
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

    template <typename T, int N> constexpr tmat<T, N, N> &operator*=(tmat<T, N, N> &a, const tmat<T, N, N> &b) noexcept {
        a = a * b;
        return a;
    }

    // ---------------------------------------------------------------------
    // Linear algebra functions (names match glm).
    // ---------------------------------------------------------------------

    template <typename T, int C, int R> [[nodiscard]] constexpr tmat<T, R, C> transpose(const tmat<T, C, R> &m) noexcept {
        tmat<T, R, C> result;
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                result(c, r) = m(r, c);
            }
        }
        return result;
    }

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

    template <typename T, int N> [[nodiscard]] constexpr tmat<T, N, N> inverse(const tmat<T, N, N> &m) noexcept {
        return adjoint(m) / determinant(m);
    }

    template <typename T, int N> [[nodiscard]] constexpr tmat<T, N, N> inverse(const tmat<T, N, N> &m, T det) noexcept {
        return adjoint(m) / det;
    }

    // ---------------------------------------------------------------------
    // Transformation builders (glm-compatible).
    //
    // These produce matrices in the usual "right-handed, column-vector" convention,
    // meaning the resulting matrices can be multiplied on the left of a point:
    //     new_point = M * point
    // and, when uploaded to OpenGL with transpose=false, behave identically to glm.
    // ---------------------------------------------------------------------

    //! Scale an existing matrix (matches glm::scale).
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> scale(const tmat<T, 4, 4> &m, const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> result;
        result[0] = m[0] * v.x;
        result[1] = m[1] * v.y;
        result[2] = m[2] * v.z;
        result[3] = m[3];
        return result;
    }

    //! Build a scale matrix from a 3-vector.
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> scale(const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 0) = v.x;
        m(1, 1) = v.y;
        m(2, 2) = v.z;
        return m;
    }

    //! Translate an existing matrix (matches glm::translate).
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> translate(const tmat<T, 4, 4> &m, const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> result = m;
        result[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
        return result;
    }

    //! Build a translation matrix.
    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> translate(const tvec<T, 3> &v) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 3) = v.x;
        m(1, 3) = v.y;
        m(2, 3) = v.z;
        return m;
    }

    //! Right-handed rotation matrix around the X axis.
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

    //! Arbitrary-axis rotation. Matches glm::rotate(mat4(1), angle, axis).
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

    //! rotate-as-glm: applies an arbitrary-axis rotation to an existing matrix.
    template <typename T> [[nodiscard]] tmat<T, 4, 4> rotate(const tmat<T, 4, 4> &m, T radians, const tvec<T, 3> &axis) {
        return m * rotate(radians, axis);
    }

    //! Right-handed view matrix (matches glm::lookAtRH).
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

    //! Default lookAt is right-handed, matching glm.
    template <typename T> [[nodiscard]] tmat<T, 4, 4> lookAt(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH(eye, center, up);
    }

    //! Right-handed perspective projection with depth in [-1, 1] (glm default).
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

    template <typename T> [[nodiscard]] tmat<T, 4, 4> perspective(T fovyRadians, T aspect, T znear, T zfar) {
        return perspectiveRH(fovyRadians, aspect, znear, zfar);
    }

    //! Right-handed orthographic projection (depth in [-1, 1]).
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

    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> ortho(T left, T right, T bottom, T top, T znear, T zfar) noexcept {
        return orthoRH(left, right, bottom, top, znear, zfar);
    }

    // ---------------------------------------------------------------------
    // Debug output.
    // ---------------------------------------------------------------------

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

    using mat2 = tmat<float, 2, 2>;
    using mat3 = tmat<float, 3, 3>;
    using mat4 = tmat<float, 4, 4>;

    using dmat2 = tmat<double, 2, 2>;
    using dmat3 = tmat<double, 3, 3>;
    using dmat4 = tmat<double, 4, 4>;

    // ---------------------------------------------------------------------
    // Legacy PascalCase aliases. Old TMatrix<T, R, C> had R rows and C columns;
    // new tmat<T, C, R> is stored column-major but the argument order is
    // indistinguishable for square matrices (the only ones used).
    // ---------------------------------------------------------------------

    template <typename T, int R, int C> using TMatrix = tmat<T, C, R>;
    using Matrix2 = mat2;
    using Matrix3 = mat3;
    using Matrix4 = mat4;

    // ---------------------------------------------------------------------
    // Legacy matrix-builder free functions. Kept here (rather than Legacy.h)
    // so that any header pulling in just Matrix.h gets them.
    // ---------------------------------------------------------------------

    template <typename T = float, int N> [[nodiscard]] constexpr tmat<T, N, N> matIdentity() noexcept {
        return tmat<T, N, N>(T(1));
    }

    template <typename T = float> [[nodiscard]] constexpr mat2 mat2Identity() noexcept {
        return mat2(T(1));
    }

    template <typename T = float> [[nodiscard]] constexpr mat3 mat3Identity() noexcept {
        return mat3(T(1));
    }

    template <typename T = float> [[nodiscard]] constexpr mat4 mat4Identity() noexcept {
        return mat4(T(1));
    }

    template <typename T> [[nodiscard]] constexpr tmat<T, 3, 3> mat3Scaling(const tvec<T, 3> &s) noexcept {
        tmat<T, 3, 3> m(T(1));
        m(0, 0) = s.x;
        m(1, 1) = s.y;
        m(2, 2) = s.z;
        return m;
    }

    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Scaling(const tvec<T, 4> &s) noexcept {
        tmat<T, 4, 4> m(T(1));
        m(0, 0) = s.x;
        m(1, 1) = s.y;
        m(2, 2) = s.z;
        m(3, 3) = s.w;
        return m;
    }

    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Scaling(const tvec<T, 3> &s) noexcept {
        return scale(s);
    }

    template <typename T> [[nodiscard]] constexpr tmat<T, 3, 3> mat3Translation(const tvec<T, 2> &t) noexcept {
        tmat<T, 3, 3> m(T(1));
        m(0, 2) = t.x;
        m(1, 2) = t.y;
        return m;
    }

    template <typename T> [[nodiscard]] constexpr tmat<T, 4, 4> mat4Translation(const tvec<T, 3> &t) noexcept {
        return translate(t);
    }

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

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationX(T radians) {
        return rotateX<T>(radians);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationY(T radians) {
        return rotateY<T>(radians);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4RotationZ(T radians) {
        return rotateZ<T>(radians);
    }

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

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Rotation(T radians, const tvec<T, 3> &axis) {
        return rotate<T>(radians, axis);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4LookAtRH(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH<T>(eye, center, up);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4LookAt(const tvec<T, 3> &eye, const tvec<T, 3> &center, const tvec<T, 3> &up) {
        return lookAtRH<T>(eye, center, up);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Perspective(T fovRadians, T aspect, T znear, T zfar) {
        return perspectiveRH<T>(fovRadians, aspect, znear, zfar);
    }

    template <typename T = float> [[nodiscard]] tmat<T, 4, 4> mat4Ortho(const tvec<T, 3> &pmin, const tvec<T, 3> &pmax) {
        return orthoRH<T>(pmin.x, pmax.x, pmin.y, pmax.y, pmin.z, pmax.z);
    }
} // namespace xe
