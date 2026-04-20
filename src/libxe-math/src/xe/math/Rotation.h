/**
 * @file Rotation.h
 * @brief Explicit angle-axis rotation representation.
 */

#pragma once

#include "Common.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Axis-and-angle rotation representation in radians.
     * Stores the rotation as a scalar angle and a 3D axis vector. Used
     * primarily as the explicit conversion target from
     * @ref tquat::operator @c axis_angle<T>(), which is the canonical way
     * to extract a human-readable rotation from a quaternion. The axis is
     * expected to be unit length when consumed by other code; conversions
     * out of @ref tquat normalise it for the caller.
     */
    template <typename T> struct axis_angle {
        T angle;         ///< Rotation angle in radians.
        tvec<T, 3> axis; ///< Rotation axis; expected to be unit length.
    };

    // Legacy PascalCase alias. See Legacy.h.
    template <typename T> using Rotation = axis_angle<T>; ///< @deprecated Use @ref axis_angle.
} // namespace xe
