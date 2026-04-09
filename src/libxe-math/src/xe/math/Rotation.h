#pragma once

#include "Common.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-angle rotation (radians). Used as an explicit conversion
     * target from @ref tquat.
     */
    template <typename T> struct axis_angle {
        T angle;
        tvec<T, 3> axis;
    };

    // Legacy PascalCase alias. See Legacy.h.
    template <typename T> using Rotation = axis_angle<T>;
} // namespace xe
