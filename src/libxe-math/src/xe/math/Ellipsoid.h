/**
 * @file Ellipsoid.h
 * @brief Axis-aligned 3D ellipsoid primitive.
 */

#pragma once

#include <ostream>
#include <typeinfo>

#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-aligned ellipsoid in 3-space — centre and per-axis radii. Not part of glm.
     * Stored as a centre point and a "size" 3-vector whose components
     * are the radii along the @c x, @c y and @c z axes (not the diameter).
     * Used for soft collision volumes and ellipsoidal swept-shape tests
     * where a sphere is too coarse.
     */
    template <typename T> struct tellipsoid {
        tvec<T, 3> center{T(0), T(0), T(0)}; ///< Centre of the ellipsoid in world space.
        tvec<T, 3> size{T(1), T(1), T(1)};   ///< Per-axis radii (not diameters).

        /**
         * @brief Default-construct to a unit sphere centred at the origin.
         * The default size of @c (1, 1, 1) makes the ellipsoid degenerate
         * into a unit-radius sphere — useful as a starting point.
         */
        constexpr tellipsoid() noexcept = default;

        /**
         * @brief Construct from explicit centre and per-axis radii.
         * @param c Centre in world space.
         * @param s Per-axis radii.
         */
        constexpr tellipsoid(const tvec<T, 3> &c, const tvec<T, 3> &s) noexcept : center(c), size(s) {
        }
    };

    /**
     * @brief Stream insertion for @ref tellipsoid — debug print.
     * Writes the type's @c typeid name followed by the centre and size on
     * separate lines. Not intended as a serialisation format.
     * @param os Output stream.
     * @param e Ellipsoid to print.
     * @return The same stream, to allow chaining.
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tellipsoid<T> &e) {
        os << "xe::ellipsoid<" << typeid(T).name() << ">{\n";
        os << "    " << e.center << ",\n";
        os << "    " << e.size << "\n";
        os << "}";
        return os;
    }

    using ellipsoid = tellipsoid<float>;   ///< Single-precision ellipsoid.
    using dellipsoid = tellipsoid<double>; ///< Double-precision ellipsoid.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TEllipsoid = tellipsoid<T>; ///< @deprecated Use @ref tellipsoid.
    using Ellipsoid = ellipsoid;                            ///< @deprecated Use @ref ellipsoid.
    using Ellipsoidd = dellipsoid;                          ///< @deprecated Use @ref dellipsoid.
} // namespace xe
