
#ifndef __XE_MATH_RECT_HPP__
#define __XE_MATH_RECT_HPP__

#include "Boundary.h"

namespace XE {
    typedef Boundary<float, 2> Rectf;
    typedef Boundary<int, 2> Recti;

    extern template class Boundary<float, 2>;
    extern template class Boundary<double, 2>;
    extern template class Boundary<int, 2>;
} // namespace XE

#endif
