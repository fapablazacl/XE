
#include "xe/math/Common.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EqualsTest, WithNoEpsilonComparesExactly") {
    REQUIRE(xe::equals(1.0f, 1.0f, 0.0f), true);
    REQUIRE(xe::equals(1.0f, 1.0f), true);
}

TEST_CASE("EqualsTest, WithEpsilonComparesApproximetly") {
    REQUIRE(xe::equals(1.00001f, 1.0f, 0.0001f), true);
    REQUIRE(xe::equals(-1.00001f, -1.0f, 0.0001f), true);
    REQUIRE(xe::equals(1.001f, 1.0f, 0.0001f), false);
    REQUIRE(xe::equals(-1.001f, -1.0f, 0.0001f), false);
}
