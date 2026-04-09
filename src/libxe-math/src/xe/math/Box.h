#pragma once

#include <ostream>
#include <typeinfo>

#include "Boundary.h"

namespace xe {
    //! 3D axis-aligned bounding box. Not part of glm.
    template <typename T> using tbox = tboundary<T, 3>;

    using box = tbox<float>;
    using dbox = tbox<double>;
    using ibox = tbox<int>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TBox = tbox<T>;
    using Box = box;
    using Boxd = dbox;
    using Boxi = ibox;

    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tbox<T> &b) {
        os << "xe::box<" << typeid(T).name() << ">{\n";
        os << "    " << b.getMinEdge() << ",\n";
        os << "    " << b.getMaxEdge() << "\n";
        os << "}";
        return os;
    }
} // namespace xe
