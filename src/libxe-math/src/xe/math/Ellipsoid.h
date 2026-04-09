#pragma once

#include <ostream>
#include <typeinfo>

#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-aligned ellipsoid. Not part of glm.
     */
    template <typename T> struct tellipsoid {
        tvec<T, 3> center{T(0), T(0), T(0)};
        tvec<T, 3> size{T(1), T(1), T(1)};

        constexpr tellipsoid() noexcept = default;

        constexpr tellipsoid(const tvec<T, 3> &c, const tvec<T, 3> &s) noexcept : center(c), size(s) {
        }
    };

    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tellipsoid<T> &e) {
        os << "xe::ellipsoid<" << typeid(T).name() << ">{\n";
        os << "    " << e.center << ",\n";
        os << "    " << e.size << "\n";
        os << "}";
        return os;
    }

    using ellipsoid = tellipsoid<float>;
    using dellipsoid = tellipsoid<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TEllipsoid = tellipsoid<T>;
    using Ellipsoid = ellipsoid;
    using Ellipsoidd = dellipsoid;
} // namespace xe
