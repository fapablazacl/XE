
#ifndef __XE_MATH_BOX_HPP__
#define __XE_MATH_BOX_HPP__

#include "Boundary.h"

namespace XE {
    //! Bounding box in 3-space. It's a Boundary speciallization
    template<typename T>
    using Box = Boundary<T, 3>;

    //! Bounding Box in 3-space, with floating-point values
    using Boxf = Box<float>;

    //! Bounding Box in 3-space, with integer values
    using Boxi = Box<int>;

    //! Serializes the content of a Box object to an ostream.
    template<typename T>
    inline std::ostream& operator<<(std::ostream &os, const Box<T>& box) {
        os << "XE::Box<" << typeid(T).name() << "{ " << std::endl;
        os << "    " << box.minEdge << ", " << std::endl;
        os << "    " << box.maxEdge << std::endl;
        os << "}" << std::endl;

        return os;
    }
}

#endif
