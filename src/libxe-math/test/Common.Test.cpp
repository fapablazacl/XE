#include <catch2/catch_test_macros.hpp>

#include "xe/math/Common.h"

TEST_CASE("xe::equals compares exactly when epsilon is zero", "[common][equals]") {
    REQUIRE(xe::equals(1.0f, 1.0f, 0.0f));
    REQUIRE(xe::equals(1.0f, 1.0f));
}

TEST_CASE("xe::equals compares approximately within an epsilon", "[common][equals]") {
    REQUIRE(xe::equals(1.00001f, 1.0f, 0.0001f));
    REQUIRE(xe::equals(-1.00001f, -1.0f, 0.0001f));
    REQUIRE_FALSE(xe::equals(1.001f, 1.0f, 0.0001f));
    REQUIRE_FALSE(xe::equals(-1.001f, -1.0f, 0.0001f));
}

TEST_CASE("xe::pi constant", "[common][pi]") {
    REQUIRE(xe::equals(xe::pi<float>, 3.14159265f, 1e-5f));
    REQUIRE(xe::equals(xe::pi<double>, 3.141592653589793, 1e-12));
}

TEST_CASE("xe::radians and xe::degrees are inverses", "[common][angles]") {
    REQUIRE(xe::equals(xe::radians(180.0f), xe::pi<float>, 1e-5f));
    REQUIRE(xe::equals(xe::degrees(xe::pi<float>), 180.0f, 1e-4f));
    REQUIRE(xe::equals(xe::degrees(xe::radians(45.0f)), 45.0f, 1e-4f));
}

TEST_CASE("xe::lerp interpolates between endpoints", "[common][lerp]") {
    REQUIRE(xe::lerp(0.0f, 10.0f, 0.0f) == 0.0f);
    REQUIRE(xe::lerp(0.0f, 10.0f, 1.0f) == 10.0f);
    REQUIRE(xe::lerp(0.0f, 10.0f, 0.5f) == 5.0f);
    REQUIRE(xe::equals(xe::lerp(-1.0f, 1.0f, 0.25f), -0.5f, 1e-6f));
}

TEST_CASE("xe::equals and xe::radians work in constexpr context", "[common][constexpr]") {
    static_assert(xe::equals(1.0f, 1.0f));
    static_assert(!xe::equals(1.0f, 2.0f));
    static_assert(xe::radians(180.0f) > 3.14f);
    REQUIRE(true); // silence "empty test case" warning
}
