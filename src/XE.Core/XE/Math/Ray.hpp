
#ifndef __XE_MATH_RAY_HPP__
#define __XE_MATH_RAY_HPP__

#include <cassert>
#include "Vector.hpp"

namespace XE::Math {
    /**
     * @brief Ray with normalized direction in the 3-space
     */
    template<typename T>
    struct Ray {
        Vector<T, 3> Position;
        Vector<T, 3> Direction;

        /**
         * @brief Computes the point of the ray at 't' distance from the starting point, to the 
         * direction of the ray.
         */
        Vector<T, 3> ComputePoint(const T t) const {
            return Position + t*Direction;
        }

        /**
         * @brief Initializes the ray, positioned around the origin and aiming to the positive Z-axis.
         */
        Ray() {
            Position = {T(0.0), T(0.0), T(0.0)};
            Direction = {T(0.0), T(0.0), T(1.0)};    
        }
    };
}

#endif 
