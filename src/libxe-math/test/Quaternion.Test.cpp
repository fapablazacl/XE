#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Quaternion.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("quat default constructor is identity", "[quat]") {
    const xe::quat q;
    REQUIRE(q.x == 0.0f);
    REQUIRE(q.y == 0.0f);
    REQUIRE(q.z == 0.0f);
    REQUIRE(q.w == 1.0f);
}

TEST_CASE("quat scalar-first constructor matches glm order", "[quat]") {
    const xe::quat q(0.5f, 1.0f, 2.0f, 3.0f); // w, x, y, z
    REQUIRE(q.w == 0.5f);
    REQUIRE(q.x == 1.0f);
    REQUIRE(q.y == 2.0f);
    REQUIRE(q.z == 3.0f);
}

TEST_CASE("quat memory order is (x, y, z, w)", "[quat][layout]") {
    const xe::quat q(0.5f, 1.0f, 2.0f, 3.0f);
    const float *p = q.data();
    REQUIRE(p[0] == 1.0f); // x
    REQUIRE(p[1] == 2.0f); // y
    REQUIRE(p[2] == 3.0f); // z
    REQUIRE(p[3] == 0.5f); // w
}

TEST_CASE("angleAxis builds a unit quaternion", "[quat][angleAxis]") {
    const xe::quat q = xe::angleAxis(xe::radians(90.0f), xe::vec3{0.0f, 0.0f, 1.0f});
    REQUIRE_THAT(xe::length(q), WithinRel(1.0f, 1e-6f));
}

TEST_CASE("quat * vec3 rotates the vector", "[quat][rotate]") {
    // 90 degrees around Z should map (1,0,0) -> (0,1,0).
    const xe::quat q = xe::angleAxis(xe::radians(90.0f), xe::vec3{0.0f, 0.0f, 1.0f});
    const xe::vec3 x{1.0f, 0.0f, 0.0f};
    const auto rotated = q * x;
    REQUIRE_THAT(rotated.x, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(rotated.y, WithinRel(1.0f, 1e-5f));
    REQUIRE_THAT(rotated.z, WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("conjugate flips the imaginary part", "[quat]") {
    const xe::quat q(0.5f, 1.0f, 2.0f, 3.0f);
    const auto c = xe::conjugate(q);
    REQUIRE(c.w == 0.5f);
    REQUIRE(c.x == -1.0f);
    REQUIRE(c.y == -2.0f);
    REQUIRE(c.z == -3.0f);
}

TEST_CASE("dot product on quaternions", "[quat][dot]") {
    const xe::quat a(1.0f, 0.0f, 0.0f, 0.0f); // identity (w=1)
    const xe::quat b(0.0f, 1.0f, 0.0f, 0.0f); // pure i
    REQUIRE(xe::dot(a, a) == 1.0f);
    REQUIRE(xe::dot(a, b) == 0.0f);
}

TEST_CASE("normalize produces a unit quaternion", "[quat][normalize]") {
    const xe::quat q(2.0f, 0.0f, 0.0f, 0.0f);
    const auto n = xe::normalize(q);
    REQUIRE_THAT(xe::length(n), WithinRel(1.0f, 1e-6f));
}

TEST_CASE("legacy Quat and uppercase .W/.X/.Y/.Z still work", "[quat][legacy]") {
    const xe::Quat q(0.5f, 1.0f, 2.0f, 3.0f);
    REQUIRE(q.W == 0.5f);
    REQUIRE(q.X == 1.0f);
    REQUIRE(q.Y == 2.0f);
    REQUIRE(q.Z == 3.0f);
}
