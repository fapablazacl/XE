
#include <catch.hpp>
#include <XE/Math/Vector.hpp>

TEST_CASE("XE::Math::Vector<T, N>") {
    SECTION("constructor for 2 components should set the parameters correctly") {
        XE::Math::Vector2f v1 = {1.0f, 2.0f};

        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
    }

    SECTION("constructor for 3 components should set the parameters correctly") {
        XE::Math::Vector3f v1 = {1.0f, 2.0f, 3.0f};

        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
        REQUIRE(v1.Z == 3.0f);
    }

    SECTION("constructor for 4 components should set the parameters correctly") {
        XE::Math::Vector4f v1 = {1.0f, 2.0f, 3.0f, 4.0f};

        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
        REQUIRE(v1.Z == 3.0f);
        REQUIRE(v1.W == 4.0f);
    }
}
