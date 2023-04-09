
#ifndef __XE_MATH_BOX_HPP__
#define __XE_MATH_BOX_HPP__

#include "Boundary.h"

namespace XE {
    extern template class Boundary<float, 3>;
    extern template class Boundary<double, 3>;
    extern template class Boundary<int, 3>;

    //! Bounding box in 3-space. It's a Boundary specialization
    template <typename T> using Box = Boundary<T, 3>;

    //! Bounding Box in 3-space, with floating-point values
    using Boxf = Box<float>;

    //! Bounding Box in 3-space, with integer values
    using Boxi = Box<int>;

    //! Serializes the content of a Box object to an ostream.
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const Box<T> &box) {
        os << "XE::Box<" << typeid(T).name() << ">{ " << std::endl;
        os << "    " << box.getMinEdge() << ", " << std::endl;
        os << "    " << box.getMaxEdge() << std::endl;
        os << "}" << std::endl;

        return os;
    }
} // namespace XE

#endif
