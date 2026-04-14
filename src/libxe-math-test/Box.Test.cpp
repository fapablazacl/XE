#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "xe/math/Box.h"

TEST_CASE("box default constructor zeroes the edges", "[box]") {
    const xe::box b;
    REQUIRE(b.getMinEdge() == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(b.getMaxEdge() == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(b.isValid());
}

TEST_CASE("box constructor from one vector", "[box]") {
    const xe::box b{xe::vec3{1.0f, 2.0f, 3.0f}};
    REQUIRE(b.getMinEdge() == xe::vec3{1.0f, 2.0f, 3.0f});
    REQUIRE(b.getMaxEdge() == xe::vec3{1.0f, 2.0f, 3.0f});
    REQUIRE(b.isValid());
}

TEST_CASE("box constructor from two vectors normalizes min/max", "[box]") {
    const xe::box b{xe::vec3{1.0f, 2.0f, -3.0f}, xe::vec3{-1.0f, 8.0f, 3.0f}};
    REQUIRE(b.getMinEdge() == xe::vec3{-1.0f, 2.0f, -3.0f});
    REQUIRE(b.getMaxEdge() == xe::vec3{1.0f, 8.0f, 3.0f});
    REQUIRE(b.isValid());
}

TEST_CASE("box center is the midpoint", "[box]") {
    const xe::box b{xe::vec3{-1.0f, -1.0f, -1.0f}, xe::vec3{1.0f, 1.0f, 1.0f}};
    REQUIRE(b.getCenter() == xe::vec3{0.0f, 0.0f, 0.0f});
    REQUIRE(b.getSize() == xe::vec3{2.0f, 2.0f, 2.0f});
}

TEST_CASE("box::isInside", "[box]") {
    const xe::box b{xe::vec3{-1.0f, -1.0f, -1.0f}, xe::vec3{1.0f, 1.0f, 1.0f}};
    REQUIRE(b.isInside({0.0f, 0.0f, 0.0f}));
    REQUIRE(b.isInside({1.0f, 1.0f, 1.0f}));
    REQUIRE_FALSE(b.isInside({2.0f, 0.0f, 0.0f}));
}

TEST_CASE("legacy Box alias still works", "[box][legacy]") {
    const xe::Box b;
    (void)b;
    REQUIRE(true);
}
