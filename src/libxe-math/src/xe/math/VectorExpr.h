/**
 * @file VectorExpr.h
 * @brief Experimental expression-template vector arithmetic.
 *
 * @warning This header is kept for reference only. It is not wired into
 * xe::vec or any consumer code, and its interface is unstable. The live
 * vector type is @ref xe::tvec in Vector.h.
 */

#pragma once

#include <cmath>
#include <functional>

namespace xe {
    //! Sum of all components of a vector expression.
    template <typename VectorExpr> auto expr_sum(VectorExpr expression) {
        auto result = expression[0];
        for (int i = 1; i < VectorExpr::vector_type::size; ++i) {
            result += expression[i];
        }
        return result;
    }

    template <typename VectorExprLeft, typename VectorExprRight> auto expr_dot(VectorExprLeft v1, VectorExprRight v2) {
        return expr_sum(v1 * v2);
    }

    template <typename VectorExpr> auto expr_length2(VectorExpr v) {
        return expr_sum(v * v);
    }

    template <typename VectorExpr> auto expr_length(VectorExpr v) {
        return std::sqrt(expr_length2(v));
    }

    template <typename VectorExprLeft, typename VectorExprRight, typename BinaryOperator> class VectorBinaryExpr {
    public:
        using vector_type = typename VectorExprLeft::vector_type;

        VectorBinaryExpr(VectorExprLeft left, VectorExprRight right) : m_left(left), m_right(right) {
        }

        auto operator[](int i) const {
            return m_operator(m_left[i], m_right[i]);
        }

    private:
        VectorExprLeft m_left;
        VectorExprRight m_right;
        BinaryOperator m_operator = BinaryOperator();
    };

    template <typename VectorExpr, typename UnaryOperator> class VectorUnaryExpr {
    public:
        using vector_type = typename VectorExpr::vector_type;

        explicit VectorUnaryExpr(VectorExpr v) : m_vector(v) {
        }

        auto operator[](int i) const {
            return m_operator(m_vector[i]);
        }

    private:
        VectorExpr m_vector;
        UnaryOperator m_operator = UnaryOperator();
    };

    template <typename T> struct expr_identity {
        T operator()(T value) const {
            return value;
        }
    };

    template <typename T> struct expr_negate {
        T operator()(T value) const {
            return -value;
        }
    };
} // namespace xe
