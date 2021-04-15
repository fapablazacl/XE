
#ifndef __XE_MATH_RAY_HPP__
#define __XE_MATH_RAY_HPP__

#include <cassert>
#include "Vector.hpp"

namespace XE {
    /**
     * @brief Ray with normalized direction in the 3-space
     */
    template<typename T>
    struct Ray {
        Vector<T, 3> position;
        Vector<T, 3> direction;

        /**
         * @brief Computes the point of the ray at 't' distance from the starting point, to the 
         * direction of the ray.
         */
        Vector<T, 3> computePoint(const T t) const {
            return position + t*direction;
        }

        /**
         * @brief Initializes the ray, positioned around the origin and aiming to the positive Z-axis.
         */
        Ray() {
            position = {T(0.0), T(0.0), T(0.0)};
            direction = {T(0.0), T(0.0), T(1.0)};    
        }
    };
}

#endif 
