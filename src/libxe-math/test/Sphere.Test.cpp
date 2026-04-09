#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Sphere.h"

using Catch::Matchers::WithinAbs;

TEST_CASE("sphere default constructor is unit sphere at origin", "[sphere]") {
    const xe::sphere s;
    REQUIRE(s.center == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(s.radius == 1.0f);
}

TEST_CASE("sphere constructor with center and radius", "[sphere]") {
    const xe::sphere s{xe::vec3{1.0f, 2.0f, 3.0f}, 4.0f};
    REQUIRE(s.center == xe::vec3{1.0f, 2.0f, 3.0f});
    REQUIRE(s.radius == 4.0f);
}

TEST_CASE("ray-sphere hit test for a direct hit", "[sphere][ray]") {
    const xe::sphere s{xe::vec3{0.0f, 0.0f, 5.0f}, 1.0f};
    const xe::ray r{xe::vec3{0.0f, 0.0f, 0.0f}, xe::vec3{0.0f, 0.0f, 1.0f}};
    const float t = xe::test(s, r);
    REQUIRE_THAT(t, WithinAbs(4.0f, 1e-5f));
}

TEST_CASE("legacy Sphere alias still works", "[sphere][legacy]") {
    const xe::Sphere s;
    REQUIRE(s.radius == 1.0f);
}
