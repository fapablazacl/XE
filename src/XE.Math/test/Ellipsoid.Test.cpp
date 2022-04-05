
#include <XE/Math/Ellipsoid.h>
#include "Common.h"
#include <sstream>


TEST_CASE("Ellipsoid's constructors should create them according to the User specifications", "[Ellipsoid]") {
    SECTION("its default constructor should construct a Ellipsoid at the origin and with radious 1, equivalent to a sphere") {
        XE::Ellipsoid<float> e1;

        REQUIRE(e1.center == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(e1.size == XE::Vector3f{1.0f, 1.0f, 1.0f});
    }

    SECTION("its constructor with 2-params should construct a Ellipsoid with the specified origin and sizes") {
        XE::Ellipsoid<float> e1{{1.0f, 2.0f, -4.0f}, {4.0f, 2.0f, 1.0f}};

        REQUIRE(e1.center == XE::Vector3f{1.0f, 2.0f, -4.0f});
        REQUIRE(e1.size == XE::Vector3f{4.0f, 2.0f, 1.0f});
    }
}

TEST_CASE("Ellipsoid can be Serialized to an Output Stream", "[Ellipsoid]") {
    SECTION("serialization should generate a non-empty string") {
        XE::Ellipsoid<float> ellipsoid;

        std::stringstream ss;
        ss << ellipsoid;

        REQUIRE(ss.str() != "");
    }
}
