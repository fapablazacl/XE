
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

    SECTION("4-param constructor generate a plane with the specified components") {
        XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

        REQUIRE(subject.a == 1.0f);
        REQUIRE(subject.b == 2.0f);
        REQUIRE(subject.c == 3.0f);
        REQUIRE(subject.d == 4.0f);
    }
}
