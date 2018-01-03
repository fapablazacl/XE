
#include <catch.hpp>
#include <XE/Math/Vector.hpp>

TEST_CASE("XE::Math::Vector<T, N>") {
    SECTION("constructor should set the parameters correctly") {
        XE::Math::Vector3f v1 = {1.0f, 2.0f, 3.0f};

        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
        REQUIRE(v1.Z == 3.0f);
    }
}
