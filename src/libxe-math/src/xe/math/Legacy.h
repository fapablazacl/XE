/**
 * @file Legacy.h
 * @brief Deprecated PascalCase aliases for xe::math.
 *
 * This header forwards the old xe-math API surface (Vector3, Matrix4, Quat,
 * Ray, Sphere, matIdentity, mat4Perspective, norm, etc.) to the new
 * glm-style primitives (@ref xe::vec3, @ref xe::mat4, @ref xe::quat, …)
 * defined in the other headers. It exists purely to keep existing call
 * sites in libxe-core, libxe-scene, libxe-geometry, libxe-gl, apostate,
 * capybaria and xe-gltf-view compiling without an immediate full
 * migration.
 *
 * All aliases declared here are deprecated; consumers should migrate to
 * the new lowercase names in a follow-up pass and this file will then be
 * removed entirely.
 *
 * @note Include @ref Math.h instead of this file directly; @ref Math.h
 * pulls in the canonical types first and then this file last so the
 * forwarders see fully-defined target types.
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

    template <typename T, int N> using TVector = tvec<T, N>; ///< @deprecated Use @ref tvec.
    template <typename T> using TVector2 = tvec<T, 2>;       ///< @deprecated Use @ref tvec<T,2>.
    template <typename T> using TVector3 = tvec<T, 3>;       ///< @deprecated Use @ref tvec<T,3>.
    template <typename T> using TVector4 = tvec<T, 4>;       ///< @deprecated Use @ref tvec<T,4>.

    using Vector2 = vec2; ///< @deprecated Use @ref vec2.
    using Vector3 = vec3; ///< @deprecated Use @ref vec3.
    using Vector4 = vec4; ///< @deprecated Use @ref vec4.

    using Vector2i = ivec2; ///< @deprecated Use @ref ivec2.
    using Vector3i = ivec3; ///< @deprecated Use @ref ivec3.
    using Vector4i = ivec4; ///< @deprecated Use @ref ivec4.

    using Vector2d = dvec2; ///< @deprecated Use @ref dvec2.
    using Vector3d = dvec3; ///< @deprecated Use @ref dvec3.
    using Vector4d = dvec4; ///< @deprecated Use @ref dvec4.

    // ---------------------------------------------------------------------
    // Matrix aliases. Old TMatrix<T, R, C> had R rows and C columns; new
    // tmat<T, C, R> stores C columns of R-component vectors. For square
    // matrices (the only ones used) the argument order is indistinguishable.
    // ---------------------------------------------------------------------

    template <typename T, int R, int C> using TMatrix = tmat<T, C, R>; ///< @deprecated Use @ref tmat. Note column-major order.

    using Matrix2 = mat2; ///< @deprecated Use @ref mat2.
    using Matrix3 = mat3; ///< @deprecated Use @ref mat3.
    using Matrix4 = mat4; ///< @deprecated Use @ref mat4.

    // ---------------------------------------------------------------------
    // Quaternion aliases.
    // ---------------------------------------------------------------------

    template <typename T> using TQuaternion = tquat<T>; ///< @deprecated Use @ref tquat.
    using Quat = quat;   ///< @deprecated Use @ref quat.
    using Quatd = dquat; ///< @deprecated Use @ref dquat.

    // ---------------------------------------------------------------------
    // Geometry aliases.
    // ---------------------------------------------------------------------

    template <typename T> using TRay = tray<T>; ///< @deprecated Use @ref tray.
    using Ray = ray;   ///< @deprecated Use @ref ray.
    using Rayd = dray; ///< @deprecated Use @ref dray.

    template <typename T> using TSphere = tsphere<T>; ///< @deprecated Use @ref tsphere.
    using Sphere = sphere;   ///< @deprecated Use @ref sphere.
    using Sphered = dsphere; ///< @deprecated Use @ref dsphere.

    template <typename T> using TBox = tbox<T>; ///< @deprecated Use @ref tbox.
    using Box = box;   ///< @deprecated Use @ref box.
    using Boxd = dbox; ///< @deprecated Use @ref dbox.
    using Boxi = ibox; ///< @deprecated Use @ref ibox.

    template <typename T, int N> using TBoundary = tboundary<T, N>; ///< @deprecated Use @ref tboundary.

    template <typename T> using TPlane = tplane<T>; ///< @deprecated Use @ref tplane.
    using Plane = plane;   ///< @deprecated Use @ref plane.
    using Planed = dplane; ///< @deprecated Use @ref dplane.

    template <typename T> using TTriangle = ttriangle<T>; ///< @deprecated Use @ref ttriangle.
    using Triangle = triangle;   ///< @deprecated Use @ref triangle.
    using Triangled = dtriangle; ///< @deprecated Use @ref dtriangle.

    template <typename T> using TEllipsoid = tellipsoid<T>; ///< @deprecated Use @ref tellipsoid.
    using Ellipsoid = ellipsoid;   ///< @deprecated Use @ref ellipsoid.
    using Ellipsoidd = dellipsoid; ///< @deprecated Use @ref dellipsoid.

    template <typename T> using TRange = trange<T>; ///< @deprecated Use @ref trange.
    using Range = range;   ///< @deprecated Use @ref range.
    using Ranged = drange; ///< @deprecated Use @ref drange.
    using Rangei = irange; ///< @deprecated Use @ref irange.

    using Rectf = trect<float>; ///< @deprecated Use @ref rect.
    using Recti = trect<int>;   ///< @deprecated Use @ref irect.

    template <typename T> using Rotation = axis_angle<T>; ///< @deprecated Use @ref axis_angle.

    // ---------------------------------------------------------------------
    // Free-function forwarders. Vector forms of norm/norm2/maximize/minimize
    // live in Vector.h so they're visible to any header that only includes
    // Vector.h. Here we only supply the quaternion overloads.
    // ---------------------------------------------------------------------

    /**
     * @brief Legacy quaternion magnitude — forwards to @ref length.
     * @deprecated Use @ref length(const tquat<T>&) instead.
     * @see length
     * @param q Input quaternion.
     * @return Euclidean magnitude of @p q.
     */
    template <typename T> [[nodiscard]] T norm(const tquat<T> &q) {
        return length(q);
    }

    /**
     * @brief Legacy quaternion squared magnitude — forwards to @ref length2.
     * @deprecated Use @ref length2(const tquat<T>&) instead.
     * @see length2
     * @param q Input quaternion.
     * @return Squared magnitude of @p q.
     */
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

    /**
     * @brief Legacy: identity quaternion factory.
     * @deprecated Default-construct a @ref tquat instead — @c tquat() already returns the identity.
     * @return The identity quaternion @c (1, 0, 0, 0).
     */
    template <typename T = float> [[nodiscard]] constexpr tquat<T> quatId() noexcept {
        return tquat<T>(T(1), T(0), T(0), T(0));
    }

    /**
     * @brief Legacy: zero quaternion factory.
     * @deprecated Construct @c tquat(T(0), T(0), T(0), T(0)) directly when a zero is actually needed.
     * @return The zero quaternion @c (0, 0, 0, 0).
     */
    template <typename T = float> [[nodiscard]] constexpr tquat<T> quatZero() noexcept {
        return tquat<T>(T(0), T(0), T(0), T(0));
    }

    /**
     * @brief Legacy: build a right-handed rotation quaternion from axis and angle.
     * @deprecated Use @ref angleAxis instead.
     * @see angleAxis
     * @param axis Unit-length rotation axis.
     * @param radians Angle in radians.
     * @return Unit quaternion representing the rotation.
     */
    template <typename T = float> [[nodiscard]] tquat<T> quatRotationRH(const tvec<T, 3> &axis, T radians) {
        return angleAxis<T>(radians, axis);
    }

    /**
     * @brief Legacy: build a left-handed rotation quaternion from axis and angle.
     * Implemented by negating the angle before delegating to @ref angleAxis.
     * @deprecated Negate the angle and use @ref angleAxis directly.
     * @see angleAxis
     * @param axis Unit-length rotation axis.
     * @param radians Angle in radians (will be negated to flip handedness).
     * @return Unit quaternion representing the left-handed rotation.
     */
    template <typename T = float> [[nodiscard]] tquat<T> quatRotationLH(const tvec<T, 3> &axis, T radians) {
        return angleAxis<T>(-radians, axis);
    }

    /**
     * @brief Legacy: rotate a vector by a quaternion.
     * @deprecated Use the @c q @c * @c v operator overload directly.
     * @see operator*(const tquat<T>&, const tvec<T,3>&)
     * @param q Unit quaternion.
     * @param v Vector to rotate.
     * @return The rotated vector.
     */
    template <typename T = float> [[nodiscard]] tvec<T, 3> transform(const tquat<T> &q, const tvec<T, 3> &v) {
        return q * v;
    }
} // namespace xe
