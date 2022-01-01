
#include <XE/Math/Plane.h>
#include <catch2/catch_all.hpp>

TEST_CASE("Plane constructors should", "[Plane]") {
    SECTION("default constructor generate a XZ plane, pointing to the positive Y-axis") {
        XE::Planef subject;

        REQUIRE(subject.a == 0.0f);
        REQUIRE(subject.b == 1.0f);
        REQUIRE(subject.c == 0.0f);
        REQUIRE(subject.d == 0.0f);
    }
}
