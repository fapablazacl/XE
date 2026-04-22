/**
 * @file Vector.EdgeCases.Test.cpp
 * @brief Pin down xe::vec3 behaviour at numerical edge cases.
 *
 * Covers inputs that random sampling almost never hits — zero vectors, NaN,
 * +/-Inf, denormals, and magnitudes near the @c float overflow boundary.
 * These tests document *current* behaviour; if one fails after a refactor,
 * the intent is that a reviewer decide whether the new behaviour is the
 * desired one (and update the test) or whether the refactor regressed.
 */

#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Vector.h"

using Catch::Matchers::WithinAbs;

namespace {
    constexpr float kInf = std::numeric_limits<float>::infinity();
    const float kNaN = std::numeric_limits<float>::quiet_NaN();
} // namespace

TEST_CASE("length/length2 of the zero vector is exactly 0", "[vector][edge][zero]") {
    const xe::vec3 z{0.0f, 0.0f, 0.0f};
    REQUIRE(xe::length2(z) == 0.0f);
    REQUIRE(xe::length(z) == 0.0f);
}

TEST_CASE("normalize of the zero vector produces NaN components", "[vector][edge][zero]") {
    // Documented behaviour: normalize divides by length, which is 0 for the
    // zero vector, yielding NaN. Pin this down so a future refactor doesn't
    // silently change it without the reviewer noticing.
    const xe::vec3 z{0.0f, 0.0f, 0.0f};
    const xe::vec3 n = xe::normalize(z);
    REQUIRE(std::isnan(n.x));
    REQUIRE(std::isnan(n.y));
    REQUIRE(std::isnan(n.z));
}

TEST_CASE("NaN components propagate through arithmetic", "[vector][edge][nan]") {
    const xe::vec3 a{1.0f, kNaN, 3.0f};
    const xe::vec3 b{4.0f, 5.0f, 6.0f};

    const auto sum = a + b;
    REQUIRE(sum.x == 5.0f);
    REQUIRE(std::isnan(sum.y));
    REQUIRE(sum.z == 9.0f);

    REQUIRE(std::isnan(xe::dot(a, b)));
    REQUIRE(std::isnan(xe::length2(a)));
}

TEST_CASE("Infinite components propagate through arithmetic", "[vector][edge][inf]") {
    const xe::vec3 a{kInf, 0.0f, 0.0f};
    const xe::vec3 b{1.0f, 2.0f, 3.0f};

    const auto sum = a + b;
    REQUIRE(sum.x == kInf);
    REQUIRE(sum.y == 2.0f);
    REQUIRE(sum.z == 3.0f);

    REQUIRE(xe::dot(a, b) == kInf);
    REQUIRE(xe::length2(a) == kInf);
    REQUIRE(xe::length(a) == kInf);
}

TEST_CASE("length2 overflows silently for huge magnitudes", "[vector][edge][overflow]") {
    // 1e20f squared is ~1e40, past float's ~3.4e38 limit. The dot-product
    // accumulation saturates to +Inf. Flag this so callers know length2 is
    // not safe for pre-check against extreme inputs.
    const xe::vec3 big{1e20f, 0.0f, 0.0f};
    REQUIRE(xe::length2(big) == kInf);
    REQUIRE(xe::length(big) == kInf);
}

TEST_CASE("length of a tiny non-zero vector stays finite", "[vector][edge][tiny]") {
    // 1e-20 squared is ~1e-40, which is a subnormal float (not flushed to
    // zero by default). length2 retains the subnormal and length recovers
    // approximately 1e-20. The assertion here is "no surprises" — the
    // result is finite and non-negative, even if precision is degraded.
    const xe::vec3 tiny{1e-20f, 0.0f, 0.0f};
    const float l2 = xe::length2(tiny);
    const float l = xe::length(tiny);
    REQUIRE(std::isfinite(l2));
    REQUIRE(l2 >= 0.0f);
    REQUIRE(std::isfinite(l));
    REQUIRE(l >= 0.0f);
}

TEST_CASE("cross product of parallel vectors is zero", "[vector][edge][cross]") {
    const xe::vec3 a{1.0f, 2.0f, 3.0f};
    const xe::vec3 b = 5.0f * a; // parallel — same direction
    const auto c = xe::cross(a, b);
    REQUIRE_THAT(c.x, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(c.y, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(c.z, WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("cross product of anti-parallel vectors is zero", "[vector][edge][cross]") {
    const xe::vec3 a{1.0f, 2.0f, 3.0f};
    const auto c = xe::cross(a, -a);
    REQUIRE_THAT(c.x, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(c.y, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(c.z, WithinAbs(0.0f, 1e-6f));
}
