#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Triangle.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("triangle stores its three points", "[triangle]") {
    const xe::triangle t{xe::vec3{1.0f, 0.0f, 0.0f}, xe::vec3{0.0f, 1.0f, 0.0f}, xe::vec3{0.0f, 0.0f, 1.0f}};
    REQUIRE(t.p1 == xe::vec3{1.0f, 0.0f, 0.0f});
    REQUIRE(t.p2 == xe::vec3{0.0f, 1.0f, 0.0f});
    REQUIRE(t.p3 == xe::vec3{0.0f, 0.0f, 1.0f});
}

TEST_CASE("triangle normal of an XY-plane triangle is +Z", "[triangle][normal]") {
    const xe::triangle t{xe::vec3{0.0f, 0.0f, 0.0f}, xe::vec3{1.0f, 0.0f, 0.0f}, xe::vec3{0.0f, 1.0f, 0.0f}};
    const auto n = t.computeNormal();
    REQUIRE_THAT(n.x, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(n.y, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(n.z, WithinAbs(1.0f, 1e-6f));
}

TEST_CASE("legacy Triangle alias still works", "[triangle][legacy]") {
    const xe::Triangle t;
    (void)t;
    REQUIRE(true);
}
