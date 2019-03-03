
#include <catch.hpp>
#include <XE/Math/Common.hpp>

TEST_CASE("Mathematical common functions") {
    SECTION("XE::equals") {
        SECTION("should behave exact like the == operator for epsilon values") {
            REQUIRE(XE::equals(1.0f, 1.0f, 0.0f) == true);
            REQUIRE((1.0f == 1.0f) == true);
        }

        SECTION("should have the tolareance of the supplied Epsilon value used for comparisons") {
            REQUIRE(XE::equals(1.00001f, 1.0f, 0.0001f) == true);
            REQUIRE(XE::equals(-1.00001f, -1.0f, 0.0001f) == true);
            REQUIRE(XE::equals(1.001f, 1.0f, 0.0001f) == false);
            REQUIRE(XE::equals(-1.001f, -1.0f, 0.0001f) == false);
        }
    }
}
