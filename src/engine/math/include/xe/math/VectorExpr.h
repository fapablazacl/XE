/**
* @file VectorExpr.h
* @brief A XE::Vector implementation using expression templates for high-performance
*/

#ifndef _XE_MATH_VECTOREXPR_HPP__
#define _XE_MATH_VECTOREXPR_HPP__

#include <cmath>
#include <functional>

namespace XE {
   /**
    * @brief Computes the sum of all
    */
   template<typename VectorExpression>
   auto sum(VectorExpression expression) {
       auto result = expression[0];

       for (int i=1; i<VectorExpression::vector_type::size; i++) {
           result += expression[i];
       }

       return result;
   }


   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto dot(VectorExpressionLeft v1, VectorExpressionRight v2) {
       return sum(v1 * v2);
   }


   /*
   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto cross(VectorExpressionLeft v1, VectorExpressionRight v2) {
       constexpr int size = VectorExpressionLeft::vector_type::size;

       if constexpr (size == 3) {
           return
       }
   }
   */


   template<typename VectorExpression>
   auto norm2(VectorExpression v) {
       return sum(v * v);
   }


   template<typename VectorExpression>
   auto norm(VectorExpression v) {
       return std::sqrt(norm2(v));
   }

   template<
       typename VectorExpressionLeft,
       typename VectorExpressionRight,
       typename BinaryOperator
       >
   class VectorBinaryExpression {
   public:
       typedef typename VectorExpressionLeft::vector_type vector_type;

   public:
       VectorBinaryExpression(VectorExpressionLeft left, VectorExpressionRight right)
           : m_left(left), m_right(right) {}

       auto operator[] (const int i) const {
           return m_operator(m_left[i], m_right[i]);
       }

   private:
       VectorExpressionLeft m_left;
       VectorExpressionRight m_right;
       BinaryOperator m_operator = BinaryOperator();
   };


   template<
       typename VectorExpression,
       typename UnaryOperator
       >
   class VectorUnaryExpression {
   public:
       typedef typename VectorExpression::vector_type vector_type;

   public:
       VectorUnaryExpression(VectorExpression vector) : m_vector(vector) {}

       auto operator[] (const int i) const {
           return m_operator(m_vector[i]);
       }

   private:
       VectorExpression m_vector;
       UnaryOperator m_operator = UnaryOperator();
   };


   template<typename T>
   struct identity {
       T operator() (const T value) const {
           return value;
       }
   };


   template<typename VectorExpression>
   auto operator+ (VectorExpression expression) {
       return VectorUnaryExpression<
           VectorExpression,
           identity< typename VectorExpression::vector_type::type >
           >(expression);
   }


   template<typename T>
   struct negate {
       T operator() (const T value) const {
           return -value;
       }
   };


   template<typename VectorExpression>
   auto operator- (VectorExpression expression) {
       return VectorUnaryExpression<
           VectorExpression,
           negate< typename VectorExpression::vector_type::type >
           >(expression);
   }


   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto operator+ (VectorExpressionLeft left, VectorExpressionRight right) {
       return VectorBinaryExpression<
           VectorExpressionLeft,
           VectorExpressionRight,
           std::plus< typename VectorExpressionLeft::vector_type::type >
           >(left, right);
   }


   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto operator- (VectorExpressionLeft left, VectorExpressionRight right) {
       return VectorBinaryExpression<
           VectorExpressionLeft,
           VectorExpressionRight,
           std::minus< typename VectorExpressionLeft::vector_type::type >
           >(left, right);
   }


   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto operator* (VectorExpressionLeft left, VectorExpressionRight right) {
       return VectorBinaryExpression<
           VectorExpressionLeft,
           VectorExpressionRight,
           std::multiplies< typename VectorExpressionLeft::vector_type::type >
           >(left, right);
   }


   template<typename VectorExpressionLeft, typename VectorExpressionRight>
   auto operator/ (VectorExpressionLeft left, VectorExpressionRight right) {
       return VectorBinaryExpression<
           VectorExpressionLeft,
           VectorExpressionRight,
           std::divides< typename VectorExpressionLeft::vector_type::type >
           >(left, right);
   }
}

#endif