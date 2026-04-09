#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "xe/math/Ellipsoid.h"

TEST_CASE("ellipsoid default constructor: unit sphere at origin", "[ellipsoid]") {
    const xe::ellipsoid e;
    REQUIRE(e.center == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(e.size == xe::vec3{1.0f, 1.0f, 1.0f});
}

TEST_CASE("ellipsoid constructor from center and size", "[ellipsoid]") {
    const xe::ellipsoid e{xe::vec3{1.0f, 2.0f, -4.0f}, xe::vec3{4.0f, 2.0f, 1.0f}};
    REQUIRE(e.center == xe::vec3{1.0f, 2.0f, -4.0f});
    REQUIRE(e.size == xe::vec3{4.0f, 2.0f, 1.0f});
}

TEST_CASE("ellipsoid serializes to a non-empty string", "[ellipsoid]") {
    std::stringstream ss;
    ss << xe::ellipsoid{};
    REQUIRE_FALSE(ss.str().empty());
}

TEST_CASE("legacy Ellipsoid alias still works", "[ellipsoid][legacy]") {
    const xe::Ellipsoid e;
    (void)e;
    REQUIRE(true);
}
