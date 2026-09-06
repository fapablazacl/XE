
#include "xe/math/Common.h"
#include <catch2/catch_all.hpp>

TEST_CASE("EqualsTest, WithNoEpsilonComparesExactly") {
    REQUIRE(xe::equals(1.0f, 1.0f, 0.0f));
    REQUIRE(xe::equals(1.0f, 1.0f));
}

TEST_CASE("EqualsTest, WithEpsilonComparesApproximetly") {
    REQUIRE(xe::equals(1.00001f, 1.0f, 0.0001f));
    REQUIRE(xe::equals(-1.00001f, -1.0f, 0.0001f));
    REQUIRE_FALSE(xe::equals(1.001f, 1.0f, 0.0001f));
    REQUIRE_FALSE(xe::equals(-1.001f, -1.0f, 0.0001f));
}
