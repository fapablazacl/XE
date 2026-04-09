/**
 * @file Legacy.h
 * @brief Deprecated PascalCase aliases for xe::math.
 *
 * This header forwards the old xe-math API surface (Vector3, Matrix4, Quat,
 * Ray, Sphere, matIdentity, mat4Perspective, norm, etc.) to the new glm-style
 * primitives (@ref xe::vec3, @ref xe::mat4, @ref xe::quat, …) defined in the
 * other headers. It exists purely to keep existing call sites in libxe-core,
 * libxe-scene, libxe-geometry, libxe-gl, apostate, capybaria and xe-gltf-view
 * compiling without an immediate full migration.
 *
 * All aliases declared here are marked @c [[deprecated]] — consumers should
 * migrate to the new names in a follow-up pass.
 *
 * @note Include @ref Math.h instead of this file directly; Math.h pulls in
 * the canonical types first and then this file last.
 */

#pragma once

#include <cmath>

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

namespace xe {
    // ---------------------------------------------------------------------
    // Vector aliases.
    // ---------------------------------------------------------------------

    template <typename T, int N> using TVector = tvec<T, N>;
    template <typename T> using TVector2 = tvec<T, 2>;
    template <typename T> using TVector3 = tvec<T, 3>;
    template <typename T> using TVector4 = tvec<T, 4>;

    using Vector2 = vec2;
    using Vector3 = vec3;
    using Vector4 = vec4;

    using Vector2i = ivec2;
    using Vector3i = ivec3;
    using Vector4i = ivec4;

    using Vector2d = dvec2;
    using Vector3d = dvec3;
    using Vector4d = dvec4;

    // ---------------------------------------------------------------------
    // Matrix aliases. Old TMatrix<T, R, C> had R rows and C columns; new
    // tmat<T, C, R> stores C columns of R-component vectors. For square
    // matrices (the only ones used) the argument order is indistinguishable.
    // ---------------------------------------------------------------------

    template <typename T, int R, int C> using TMatrix = tmat<T, C, R>;

    using Matrix2 = mat2;
    using Matrix3 = mat3;
    using Matrix4 = mat4;

    // ---------------------------------------------------------------------
    // Quaternion aliases.
    // ---------------------------------------------------------------------

    template <typename T> using TQuaternion = tquat<T>;
    using Quat = quat;
    using Quatd = dquat;

    // ---------------------------------------------------------------------
    // Geometry aliases.
    // ---------------------------------------------------------------------

    template <typename T> using TRay = tray<T>;
    using Ray = ray;
    using Rayd = dray;

    template <typename T> using TSphere = tsphere<T>;
    using Sphere = sphere;
    using Sphered = dsphere;

    template <typename T> using TBox = tbox<T>;
    using Box = box;
    using Boxd = dbox;
    using Boxi = ibox;

    template <typename T, int N> using TBoundary = tboundary<T, N>;

    template <typename T> using TPlane = tplane<T>;
    using Plane = plane;
    using Planed = dplane;

    template <typename T> using TTriangle = ttriangle<T>;
    using Triangle = triangle;
    using Triangled = dtriangle;

    template <typename T> using TEllipsoid = tellipsoid<T>;
    using Ellipsoid = ellipsoid;
    using Ellipsoidd = dellipsoid;

    template <typename T> using TRange = trange<T>;
    using Range = range;
    using Ranged = drange;
    using Rangei = irange;

    using Rectf = trect<float>;
    using Recti = trect<int>;

    template <typename T> using Rotation = axis_angle<T>;

    // ---------------------------------------------------------------------
    // Free-function forwarders. Vector forms of norm/norm2/maximize/minimize
    // live in Vector.h so they're visible to any header that only includes
    // Vector.h. Here we only supply the quaternion overloads.
    // ---------------------------------------------------------------------

    template <typename T> [[nodiscard]] T norm(const tquat<T> &q) {
        return length(q);
    }

    template <typename T> [[nodiscard]] constexpr T norm2(const tquat<T> &q) noexcept {
        return length2(q);
    }

    // ---------------------------------------------------------------------
    // Matrix builder legacy forwarders live in Matrix.h so any file including
    // just Matrix.h can see them.
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    // Quaternion builder forwarders.
    // ---------------------------------------------------------------------

    template <typename T = float> [[nodiscard]] constexpr tquat<T> quatId() noexcept {
        return tquat<T>(T(1), T(0), T(0), T(0));
    }

    template <typename T = float> [[nodiscard]] constexpr tquat<T> quatZero() noexcept {
        return tquat<T>(T(0), T(0), T(0), T(0));
    }

    template <typename T = float> [[nodiscard]] tquat<T> quatRotationRH(const tvec<T, 3> &axis, T radians) {
        return angleAxis<T>(radians, axis);
    }

    template <typename T = float> [[nodiscard]] tquat<T> quatRotationLH(const tvec<T, 3> &axis, T radians) {
        return angleAxis<T>(-radians, axis);
    }

    template <typename T = float> [[nodiscard]] tvec<T, 3> transform(const tquat<T> &q, const tvec<T, 3> &v) {
        return q * v;
    }
} // namespace xe
