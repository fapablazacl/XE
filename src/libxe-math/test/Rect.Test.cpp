#include <catch2/catch_test_macros.hpp>

#include "xe/math/Rect.h"

TEST_CASE("rect default constructor is the zero rectangle", "[rect]") {
    const xe::rect r;
    REQUIRE(r.getMinEdge() == xe::vec2{0.0f, 0.0f});
    REQUIRE(r.getMaxEdge() == xe::vec2{0.0f, 0.0f});
    REQUIRE(r.isValid());
}

TEST_CASE("rect constructor from two points", "[rect]") {
    const xe::rect r{xe::vec2{1.0f, 5.0f}, xe::vec2{3.0f, 2.0f}};
    REQUIRE(r.getMinEdge() == xe::vec2{1.0f, 2.0f});
    REQUIRE(r.getMaxEdge() == xe::vec2{3.0f, 5.0f});
}

TEST_CASE("legacy Rectf alias still works", "[rect][legacy]") {
    const xe::Rectf r;
    (void)r;
    REQUIRE(true);
}
