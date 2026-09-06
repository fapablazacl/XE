
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <xe/math/Ellipsoid.h>

TEST_CASE("EllipsoidTest, DefaultConstructorInitializesAtTheOriginAndWithRadiousOne") {
    xe::Ellipsoid e1;
    REQUIRE(e1.center == xe::Vector3(0.0f, 0.0f, 0.0f));
    REQUIRE(e1.size == xe::Vector3(1.0f, 1.0f, 1.0f));
}

TEST_CASE("EllipsoidTest, ConstructorInitializesAtTheSpecifiedOriginAndRadiouses") {
    xe::Ellipsoid e1{{1.0f, 2.0f, -4.0f}, {4.0f, 2.0f, 1.0f}};
    REQUIRE(e1.center == xe::Vector3(1.0f, 2.0f, -4.0f));
    REQUIRE(e1.size == xe::Vector3(4.0f, 2.0f, 1.0f));
}

TEST_CASE("EllipsoidTest, CanBeSerializedToANonEmptyString") {
    xe::Ellipsoid ellipsoid;

    std::stringstream ss;
    ss << ellipsoid;

    EXPECT_NE(ss.str(), "");
}
