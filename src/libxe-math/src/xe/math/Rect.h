#pragma once

#include "Boundary.h"

namespace xe {
    //! 2D axis-aligned rectangle. Not part of glm.
    template <typename T> using trect = tboundary<T, 2>;

    using rect = trect<float>;
    using drect = trect<double>;
    using irect = trect<int>;

    // Legacy PascalCase aliases. See Legacy.h.
    using Rectf = trect<float>;
    using Recti = trect<int>;
} // namespace xe
