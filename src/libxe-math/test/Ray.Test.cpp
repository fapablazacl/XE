#include <catch2/catch_test_macros.hpp>

#include "xe/math/Ray.h"

TEST_CASE("ray default constructor sits at origin pointing +Z", "[ray]") {
    const xe::ray r;
    REQUIRE(r.origin == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(r.direction == xe::vec3{0.0f, 0.0f, 1.0f});
}

TEST_CASE("ray pointAt walks along the direction", "[ray]") {
    const xe::ray r{xe::vec3{1.0f, 2.0f, 3.0f}, xe::vec3{0.0f, 0.0f, 1.0f}};
    REQUIRE(r.pointAt(5.0f) == xe::vec3{1.0f, 2.0f, 8.0f});
    REQUIRE(r.pointAt(0.0f) == r.origin);
}

TEST_CASE("legacy Ray alias still works", "[ray][legacy]") {
    const xe::Ray r;
    REQUIRE(r.direction.z == 1.0f);
}
