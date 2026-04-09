/**
 * @file Box.h
 * @brief 3D axis-aligned bounding box — specialisation of @ref tboundary.
 */

#pragma once

#include <ostream>
#include <typeinfo>

#include "Boundary.h"

namespace xe {
    /**
     * @brief 3D axis-aligned bounding box. xe-specific (not part of glm).
     * Implemented as a type alias for @ref tboundary "tboundary<T,3>"; all
     * box-shaped behaviour (expansion, containment, SAT intersection,
     * corner enumeration) lives in the @ref tboundary template. The alias
     * exists so application code can name the 3D case directly without
     * spelling out the dimensionality.
     */
    template <typename T> using tbox = tboundary<T, 3>;

    using box = tbox<float>;   ///< Single-precision @ref tbox alias.
    using dbox = tbox<double>; ///< Double-precision @ref tbox alias.
    using ibox = tbox<int>;    ///< Integer-coordinate @ref tbox alias.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TBox = tbox<T>; ///< @deprecated Use @ref tbox.
    using Box = box;   ///< @deprecated Use @ref box.
    using Boxd = dbox; ///< @deprecated Use @ref dbox.
    using Boxi = ibox; ///< @deprecated Use @ref ibox.

    /**
     * @brief Stream insertion for @ref tbox — debug print of min/max corners.
     * Not intended as a serialisation format; the output includes the
     * scalar type's @c typeid name and the two corners on separate lines.
     * @param os Output stream.
     * @param b Box to print.
     * @return The same stream, to allow chaining.
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tbox<T> &b) {
        os << "xe::box<" << typeid(T).name() << ">{\n";
        os << "    " << b.getMinEdge() << ",\n";
        os << "    " << b.getMaxEdge() << "\n";
        os << "}";
        return os;
    }
} // namespace xe
