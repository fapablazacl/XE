/**
 * @file Quaternion.EdgeCases.Test.cpp
 * @brief Pin down xe::quat behaviour at numerical edge cases.
 *
 * Covers zero-length axes, zero quaternions, 180-degree basis-axis
 * rotations (which random sampling rarely hits cleanly), and the
 * q*conj(q)==identity algebraic invariant on a random unit quaternion.
 */

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "MathTestHelpers.h"
#include "xe/math/Common.h"
#include "xe/math/Quaternion.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("angleAxis with zero-length axis yields a degenerate quaternion", "[quat][edge][zero-axis]") {
    // angleAxis does not normalize its axis argument; a zero axis produces
    // a quaternion whose imaginary part is zero and whose real part is
    // cos(angle/2). This is not a unit quaternion for angles other than 0.
    const xe::quat q = xe::angleAxis(xe::radians(90.0f), xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(q.x == 0.0f);
    REQUIRE(q.y == 0.0f);
    REQUIRE(q.z == 0.0f);
    REQUIRE_THAT(q.w, WithinRel(std::cos(xe::radians(45.0f)), 1e-6f));
}

TEST_CASE("normalize of the zero quaternion yields NaN components", "[quat][edge][zero-quat]") {
    // Zero quaternion has length 0, so normalize divides by zero. Document
    // the NaN result so a safety-net (clamp, return identity, etc.) would
    // require a deliberate test update.
    const xe::quat zero(0.0f, 0.0f, 0.0f, 0.0f);
    const xe::quat n = xe::normalize(zero);
    REQUIRE(std::isnan(n.x));
    REQUIRE(std::isnan(n.y));
    REQUIRE(std::isnan(n.z));
    REQUIRE(std::isnan(n.w));
}

TEST_CASE("180-degree rotations around each basis axis", "[quat][edge][basis]") {
    // 180deg around X: should map (0,1,0) -> (0,-1,0) and (0,0,1) -> (0,0,-1).
    {
        const xe::quat qx = xe::angleAxis(xe::radians(180.0f), xe::vec3{1.0f, 0.0f, 0.0f});
        const xe::vec3 ry = qx * xe::vec3{0.0f, 1.0f, 0.0f};
        const xe::vec3 rz = qx * xe::vec3{0.0f, 0.0f, 1.0f};
        REQUIRE_THAT(ry.x, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(ry.y, WithinRel(-1.0f, 1e-5f));
        REQUIRE_THAT(ry.z, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(rz.x, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(rz.y, WithinAbs(0.0f, 1e-6f));
        REQUIRE_THAT(rz.z, WithinRel(-1.0f, 1e-5f));
    }
    // 180deg around Y: maps (1,0,0) -> (-1,0,0) and (0,0,1) -> (0,0,-1).
    {
        const xe::quat qy = xe::angleAxis(xe::radians(180.0f), xe::vec3{0.0f, 1.0f, 0.0f});
        const xe::vec3 rx = qy * xe::vec3{1.0f, 0.0f, 0.0f};
        const xe::vec3 rz = qy * xe::vec3{0.0f, 0.0f, 1.0f};
        REQUIRE_THAT(rx.x, WithinRel(-1.0f, 1e-5f));
        REQUIRE_THAT(rz.z, WithinRel(-1.0f, 1e-5f));
    }
    // 180deg around Z: maps (1,0,0) -> (-1,0,0) and (0,1,0) -> (0,-1,0).
    {
        const xe::quat qz = xe::angleAxis(xe::radians(180.0f), xe::vec3{0.0f, 0.0f, 1.0f});
        const xe::vec3 rx = qz * xe::vec3{1.0f, 0.0f, 0.0f};
        const xe::vec3 ry = qz * xe::vec3{0.0f, 1.0f, 0.0f};
        REQUIRE_THAT(rx.x, WithinRel(-1.0f, 1e-5f));
        REQUIRE_THAT(ry.y, WithinRel(-1.0f, 1e-5f));
    }
}

TEST_CASE("q * conjugate(q) equals identity for a random unit quaternion", "[quat][edge][invariant]") {
    // Algebraic invariant: for any unit quaternion q, q * conj(q) = identity.
    // Cheap sanity check that the Hamilton product agrees with conjugate.
    auto rng = xe::test::seededRng();
    for (int i = 0; i < 32; ++i) {
        CAPTURE(i);
        const xe::vec3 axis = xe::test::randomUnitVec3(rng);
        std::uniform_real_distribution<float> angleDist(-6.28f, 6.28f);
        const xe::quat q = xe::angleAxis(angleDist(rng), axis);
        const xe::quat id = q * xe::conjugate(q);
        REQUIRE_THAT(id.x, WithinAbs(0.0f, 1e-5f));
        REQUIRE_THAT(id.y, WithinAbs(0.0f, 1e-5f));
        REQUIRE_THAT(id.z, WithinAbs(0.0f, 1e-5f));
        REQUIRE_THAT(id.w, WithinRel(1.0f, 1e-5f));
    }
}

TEST_CASE("identity quaternion rotates vectors unchanged", "[quat][edge][identity]") {
    const xe::quat id; // default ctor = identity
    const xe::vec3 v{1.0f, 2.0f, 3.0f};
    const xe::vec3 r = id * v;
    REQUIRE_THAT(r.x, WithinRel(1.0f, 1e-6f));
    REQUIRE_THAT(r.y, WithinRel(2.0f, 1e-6f));
    REQUIRE_THAT(r.z, WithinRel(3.0f, 1e-6f));
}
