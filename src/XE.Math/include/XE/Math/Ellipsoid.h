
#ifndef __XE_MATH_ELLIPSOID_HPP__
#define __XE_MATH_ELLIPSOID_HPP__

#include "Vector.h"

#include <limits>

namespace XE {
    /**
     * @brief Ellipsoid in the 3-space
     */
    template<typename T>
    struct Ellipsoid {
        Vector<T, 3> center;
        Vector<T, 3> size;
        
        /**
         * @brief Initializes a Ellipsoid from the given center and radius
         */
        Ellipsoid(const Vector<T, 3> &center, const Vector<T, 3> &size) {
            this->center = center;
            this->size = size;
        }
    };
}

#endif
