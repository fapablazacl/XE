/**
 * @file Plane.h
 * @brief 3D plane in scalar equation form, plus ray-plane intersection.
 */

#pragma once

#include <ostream>

#include "Ray.h"
#include "Vector.h"

namespace xe {
    /**
     * @brief Result of @ref tplane::test for a point classification query.
     * @c Front means the point lies on the side the plane normal points
     * to, @c Back means the opposite side, and @c Inside means the point
     * lies exactly on the plane (signed distance is zero).
     */
    enum class PlaneSide { Front, Back, Inside };

    /**
     * @brief Plane in 3-space, scalar equation form @f$ ax + by + cz = d @f$.
     * Not part of glm. The four scalar coefficients are stored
     * contiguously so @c data() returns a pointer suitable for direct
     * upload to GPU buffers. The normal of the plane is the vector
     * @c (a, b, c); intersection routines and the @ref intersect /
     * @ref test helpers assume the normal is unit length, so callers that
     * construct a plane manually should normalise their inputs.
     */
    template <typename T> struct tplane {
        T a = T(0); ///< First coefficient — @c x component of the normal.
        T b = T(1); ///< Second coefficient — @c y component of the normal.
        T c = T(0); ///< Third coefficient — @c z component of the normal.
        T d = T(0); ///< Plane constant; signed distance from the origin along the normal.

        /**
         * @brief Default-construct to the @c y=0 plane (XZ plane through the origin).
         */
        constexpr tplane() noexcept = default;

        /**
         * @brief Construct from explicit scalar coefficients @c (a, b, c, d).
         * @param av First coefficient.
         * @param bv Second coefficient.
         * @param cv Third coefficient.
         * @param dv Plane constant.
         */
        constexpr tplane(T av, T bv, T cv, T dv) noexcept : a(av), b(bv), c(cv), d(dv) {
        }

        /**
         * @brief Construct from a normal vector and a plane constant.
         * The components of @p n become @c (a, b, c). @p n is expected to
         * be unit length; intersection routines depend on it.
         * @param n Plane normal; should be normalised.
         * @param dv Plane constant.
         */
        constexpr tplane(const tvec<T, 3> &n, T dv) noexcept : a(n.x), b(n.y), c(n.z), d(dv) {
        }

        /**
         * @brief Get the plane's normal as a 3-vector.
         * @return The vector @c (a, b, c).
         */
        [[nodiscard]] constexpr tvec<T, 3> normal() const noexcept {
            return {a, b, c};
        }

        /**
         * @brief Signed distance of @p point from the plane.
         * Positive when the point lies on the side the normal points to
         * ("front"), negative on the opposite side, zero when the point is
         * exactly on the plane. Equivalent to @c dot(normal(), point) - d.
         * @param point Point to evaluate.
         * @return The signed distance.
         */
        [[nodiscard]] constexpr T evaluate(const tvec<T, 3> &point) const noexcept {
            return dot(normal(), point) - d;
        }

        /**
         * @brief Strict componentwise equality.
         * @param rhs Right-hand operand.
         * @return @c true when every coefficient matches exactly.
         */
        [[nodiscard]] constexpr bool operator==(const tplane<T> &rhs) const noexcept {
            return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d;
        }

        /**
         * @brief Strict componentwise inequality.
         * @param rhs Right-hand operand.
         * @return @c true when at least one coefficient differs.
         */
        [[nodiscard]] constexpr bool operator!=(const tplane<T> &rhs) const noexcept {
            return !(*this == rhs);
        }

        /**
         * @brief Test whether two planes are not parallel — i.e. they intersect along a line.
         * Implemented by checking that the dot product of the two normals
         * is not exactly @c 1, which characterises identical orientation.
         * Assumes both normals are unit-length; non-unit normals will give
         * misleading results.
         * @param other Plane to test against.
         * @return @c true when the planes are not parallel and therefore intersect along a line.
         */
        [[nodiscard]] constexpr bool intersect(const tplane<T> &other) const noexcept {
            return dot(normal(), other.normal()) != T(1);
        }

        /**
         * @brief Classify a point as in front of, behind, or on the plane.
         * @param point Point to classify.
         * @return @ref PlaneSide::Front, @ref PlaneSide::Back, or @ref PlaneSide::Inside.
         */
        [[nodiscard]] constexpr PlaneSide test(const tvec<T, 3> &point) const noexcept {
            const T det = evaluate(point);
            if (det < T(0)) {
                return PlaneSide::Back;
            }
            if (det > T(0)) {
                return PlaneSide::Front;
            }
            return PlaneSide::Inside;
        }

        /**
         * @brief Mutable pointer to the contiguous @c (a, b, c, d) storage.
         * Suitable for upload to a GPU uniform buffer.
         * @return Pointer to the first coefficient.
         */
        [[nodiscard]] constexpr T *data() noexcept {
            return &a;
        }

        /**
         * @brief Const pointer to the contiguous @c (a, b, c, d) storage.
         * @return Const pointer to the first coefficient.
         */
        [[nodiscard]] constexpr const T *data() const noexcept {
            return &a;
        }
    };

    /**
     * @brief Build a plane from a point and a (possibly unnormalised) normal.
     * Normalises @p n internally and computes the plane constant as the
     * projection of @p position onto the normalised normal, so the
     * resulting plane passes through @p position.
     * @param n Plane normal; need not be unit length.
     * @param position Any point known to lie on the plane.
     * @return The corresponding @ref tplane.
     */
    template <typename T = float> [[nodiscard]] tplane<T> planeVectorial(const tvec<T, 3> &n, const tvec<T, 3> &position) {
        const auto normal = normalize(n);
        return tplane<T>{normal, dot(position, normal)};
    }

    /**
     * @brief Build a plane from three non-collinear points.
     * Computes the normal as the normalised cross product of two edge
     * vectors, then anchors the plane through the centroid of the three
     * points. Behaviour is undefined when the points are collinear.
     * @param p1 First triangle vertex.
     * @param p2 Second triangle vertex.
     * @param p3 Third triangle vertex.
     * @return The plane that contains all three points.
     */
    template <typename T = float> [[nodiscard]] tplane<T> planeTriangle(const tvec<T, 3> &p1, const tvec<T, 3> &p2, const tvec<T, 3> &p3) {
        const auto n = normalize(cross(p2 - p1, p3 - p1));
        const auto centroid = (p1 + p2 + p3) * (T(1) / T(3));
        return planeVectorial(n, centroid);
    }

    /**
     * @brief Canonical YZ plane (@c x = 0) — normal @c (1, 0, 0).
     * @return The plane @c x = 0.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeYZ() noexcept {
        return tplane<T>{T(1), T(0), T(0), T(0)};
    }

    /**
     * @brief Canonical XZ plane (@c y = 0) — normal @c (0, 1, 0).
     * @return The plane @c y = 0.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXZ() noexcept {
        return tplane<T>{T(0), T(1), T(0), T(0)};
    }

    /**
     * @brief Canonical XY plane (@c z = 0) — normal @c (0, 0, 1).
     * @return The plane @c z = 0.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXY() noexcept {
        return tplane<T>{T(0), T(0), T(1), T(0)};
    }

    /**
     * @brief YZ plane offset to pass through @p p — normal @c (1, 0, 0).
     * @param p Anchor point; only @c p.x is used.
     * @return The plane @c x = p.x.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeYZ(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(1), T(0), T(0), p.x};
    }

    /**
     * @brief XZ plane offset to pass through @p p — normal @c (0, 1, 0).
     * @param p Anchor point; only @c p.y is used.
     * @return The plane @c y = p.y.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXZ(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(0), T(1), T(0), p.y};
    }

    /**
     * @brief XY plane offset to pass through @p p — normal @c (0, 0, 1).
     * @param p Anchor point; only @c p.z is used.
     * @return The plane @c z = p.z.
     */
    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXY(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(0), T(0), T(1), p.z};
    }

    /**
     * @brief Return the plane with its normal flipped.
     * The plane constant is preserved so the plane occupies the same
     * locus in space; only the front/back classification swaps.
     * @param p Plane to flip.
     * @return The plane @c (-a, -b, -c, d).
     */
    template <typename T> [[nodiscard]] constexpr tplane<T> invert(const tplane<T> &p) noexcept {
        return {-p.a, -p.b, -p.c, p.d};
    }

    /**
     * @brief Ray-plane intersection — returns the ray parameter at the hit.
     * Solves the linear equation for the parameter @c t at which
     * @c r.pointAt(t) satisfies the plane equation. When the ray is
     * parallel to the plane @c dem is zero and the result is a division
     * by zero (caller must guard, or detect with @c std::isfinite). The
     * caller is also responsible for rejecting hits with negative @c t if
     * "behind the origin" intersections are not desired.
     * @param p Plane to test against.
     * @param r Ray to test; @c r.direction is assumed unit-length.
     * @return Ray parameter @c t at the hit point.
     */
    template <typename T> [[nodiscard]] constexpr T test(const tplane<T> &p, const tray<T> &r) noexcept {
        const tvec<T, 3> n = p.normal();
        const T num = p.d - dot(r.origin, n);
        const T dem = dot(r.direction, n);
        return num / dem;
    }

    /**
     * @brief Stream insertion for debug output.
     * Writes the plane in the form @c "xe::tplane{ a, b, c, d }". Not
     * intended as a serialisation format.
     * @param os Output stream.
     * @param p Plane to print.
     * @return The same stream, to allow chaining.
     */
    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tplane<T> &p) {
        os << "xe::tplane{ " << p.a << ", " << p.b << ", " << p.c << ", " << p.d << " }";
        return os;
    }

    using plane = tplane<float>;   ///< Single-precision plane.
    using dplane = tplane<double>; ///< Double-precision plane.

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TPlane = tplane<T>; ///< @deprecated Use @ref tplane.
    using Plane = plane;                            ///< @deprecated Use @ref plane.
    using Planed = dplane;                          ///< @deprecated Use @ref dplane.
} // namespace xe
