/**
 * @file Rect.h
 * @brief 2D axis-aligned rectangle — specialisation of @ref tboundary.
 */

#pragma once

#include "Boundary.h"

namespace xe {
    /**
     * @brief 2D axis-aligned rectangle. xe-specific (not part of glm).
     * Implemented as a type alias for @ref tboundary "tboundary<T,2>";
     * all rectangle behaviour (expansion, containment, SAT intersection,
     * corner enumeration) lives in the @ref tboundary template. The alias
     * exists so 2D code can refer to the type by its natural name.
     */
    template <typename T> using trect = tboundary<T, 2>;

    using rect = trect<float>;   ///< Single-precision @ref trect alias.
    using drect = trect<double>; ///< Double-precision @ref trect alias.
    using irect = trect<int>;    ///< Integer-coordinate @ref trect alias.

    // Legacy PascalCase aliases. See Legacy.h.
    using Rectf = trect<float>; ///< @deprecated Use @ref rect.
    using Recti = trect<int>;   ///< @deprecated Use @ref irect.
} // namespace xe
