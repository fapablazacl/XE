
#include <catch.hpp>
#include <XE/Math/Vector.hpp>

using namespace XE;
using namespace XE::Math;

TEST_CASE("XE::Math::Vector<3, float>") {
    SECTION("constructor for 3 components should set the parameters correctly") {
        Vector3f v1 = {1.0f, 2.0f, 3.0f};

        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
        REQUIRE(v1.Z == 3.0f);
    }

    SECTION("Normalize function should return a 1-length vector") {
        Vector3f v1 = {1.0f, 1.0f, 1.0f};
        Vector3f normalized = Normalize(v1);
        
        REQUIRE(v1.X == 1.0f);
        REQUIRE(v1.Y == 2.0f);
        REQUIRE(v1.Z == 3.0f);
    }
}
