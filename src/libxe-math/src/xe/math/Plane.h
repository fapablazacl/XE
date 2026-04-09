#pragma once

#include <ostream>

#include "Ray.h"
#include "Vector.h"

namespace xe {
    enum class PlaneSide { Front, Back, Inside };

    /**
     * @brief Plane in 3-space, scalar equation form: @f$ ax + by + cz = d @f$.
     *
     * Not part of glm. Stored as four floats so @c data() yields a contiguous buffer.
     */
    template <typename T> struct tplane {
        T a = T(0);
        T b = T(1);
        T c = T(0);
        T d = T(0);

        constexpr tplane() noexcept = default;

        constexpr tplane(T av, T bv, T cv, T dv) noexcept : a(av), b(bv), c(cv), d(dv) {
        }

        constexpr tplane(const tvec<T, 3> &n, T dv) noexcept : a(n.x), b(n.y), c(n.z), d(dv) {
        }

        [[nodiscard]] constexpr tvec<T, 3> normal() const noexcept {
            return {a, b, c};
        }

        /**
         * @brief Signed distance of @p point from the plane (positive = front side).
         */
        [[nodiscard]] constexpr T evaluate(const tvec<T, 3> &point) const noexcept {
            return dot(normal(), point) - d;
        }

        [[nodiscard]] constexpr bool operator==(const tplane<T> &rhs) const noexcept {
            return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d;
        }

        [[nodiscard]] constexpr bool operator!=(const tplane<T> &rhs) const noexcept {
            return !(*this == rhs);
        }

        /**
         * @brief True when this plane and @p other are not parallel.
         * @note Assumes both normals are unit-length.
         */
        [[nodiscard]] constexpr bool intersect(const tplane<T> &other) const noexcept {
            return dot(normal(), other.normal()) != T(1);
        }

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

        [[nodiscard]] constexpr T *data() noexcept {
            return &a;
        }

        [[nodiscard]] constexpr const T *data() const noexcept {
            return &a;
        }
    };

    //! Build a plane from a point and an (unnormalized) normal.
    template <typename T = float> [[nodiscard]] tplane<T> planeVectorial(const tvec<T, 3> &n, const tvec<T, 3> &position) {
        const auto normal = normalize(n);
        return tplane<T>{normal, dot(position, normal)};
    }

    //! Build a plane from three points.
    template <typename T = float> [[nodiscard]] tplane<T> planeTriangle(const tvec<T, 3> &p1, const tvec<T, 3> &p2, const tvec<T, 3> &p3) {
        const auto n = normalize(cross(p2 - p1, p3 - p1));
        const auto centroid = (p1 + p2 + p3) * (T(1) / T(3));
        return planeVectorial(n, centroid);
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeYZ() noexcept {
        return tplane<T>{T(1), T(0), T(0), T(0)};
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXZ() noexcept {
        return tplane<T>{T(0), T(1), T(0), T(0)};
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXY() noexcept {
        return tplane<T>{T(0), T(0), T(1), T(0)};
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeYZ(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(1), T(0), T(0), p.x};
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXZ(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(0), T(1), T(0), p.y};
    }

    template <typename T = float> [[nodiscard]] constexpr tplane<T> planeXY(const tvec<T, 3> &p) noexcept {
        return tplane<T>{T(0), T(0), T(1), p.z};
    }

    template <typename T> [[nodiscard]] constexpr tplane<T> invert(const tplane<T> &p) noexcept {
        return {-p.a, -p.b, -p.c, p.d};
    }

    /**
     * @brief Ray/plane intersection — returns the ray parameter of the hit point.
     */
    template <typename T> [[nodiscard]] constexpr T test(const tplane<T> &p, const tray<T> &r) noexcept {
        const tvec<T, 3> n = p.normal();
        const T num = p.d - dot(r.origin, n);
        const T dem = dot(r.direction, n);
        return num / dem;
    }

    template <typename T> inline std::ostream &operator<<(std::ostream &os, const tplane<T> &p) {
        os << "xe::tplane{ " << p.a << ", " << p.b << ", " << p.c << ", " << p.d << " }";
        return os;
    }

    using plane = tplane<float>;
    using dplane = tplane<double>;

    // Legacy PascalCase aliases. See Legacy.h.
    template <typename T> using TPlane = tplane<T>;
    using Plane = plane;
    using Planed = dplane;
} // namespace xe
