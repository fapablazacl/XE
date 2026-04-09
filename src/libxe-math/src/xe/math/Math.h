/**
 * @file Math.h
 * @brief Umbrella include for the public xe::math API.
 *
 * Pulls in every canonical (glm-style) header in the library so consumers can
 * write a single @c #include "xe/math/Math.h" and get vectors, matrices,
 * quaternions, the boundary primitives (Box, Rect, Ray, Sphere, Plane,
 * Triangle, Ellipsoid), the Range and Rotation helpers, and the scalar
 * functions in Common.h. The deprecated PascalCase aliases in Legacy.h are
 * intentionally included last so they can reference every canonical type.
 *
 * @note Pick this header when you don't care about translation-unit cost. For
 * tighter dependencies, include only the specific headers you need.
 */

#pragma once

// Canonical (glm-style) xe::math public headers.
#include "Boundary.h"
#include "Box.h"
#include "Common.h"
#include "Ellipsoid.h"
#include "Matrix.h"
#include "Plane.h"
#include "Quaternion.h"
#include "Range.h"
#include "Ray.h"
#include "Rect.h"
#include "Rotation.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vector.h"

// Deprecated PascalCase aliases. Kept last so it sees every canonical type.
#include "Legacy.h"
