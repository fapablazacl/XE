#include <catch2/catch_test_macros.hpp>

#include "xe/math/Rotation.h"

TEST_CASE("axis_angle stores angle and axis", "[rotation]") {
    const xe::axis_angle<float> r{xe::radians(45.0f), xe::vec3{0.0f, 1.0f, 0.0f}};
    REQUIRE(r.angle > 0.0f);
    REQUIRE(r.axis == xe::vec3{0.0f, 1.0f, 0.0f});
}

TEST_CASE("legacy Rotation<T> alias still works", "[rotation][legacy]") {
    const xe::Rotation<float> r{0.0f, xe::vec3{1.0f, 0.0f, 0.0f}};
    (void)r;
    REQUIRE(true);
}
