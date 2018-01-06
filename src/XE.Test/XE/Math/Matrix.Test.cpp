
#include <catch.hpp>
#include <XE/Math/Matrix.hpp>

TEST_CASE("XE::Math::Matrix<T, N>") {
    SECTION("constructor should set the parameters correctly") {
        XE::Math::Matrix2f m = {
            1.0f, 2.0f, 
            3.0f, 4.0f
        };

        REQUIRE(m.M11 == 1.0f);
        REQUIRE(m.M12 == 2.0f);
        REQUIRE(m.M21 == 3.0f);
        REQUIRE(m.M22 == 4.0f);

        REQUIRE(m.Element[0][0] == 1.0f);
        REQUIRE(m.Element[0][1] == 2.0f);
        REQUIRE(m.Element[1][0] == 3.0f);
        REQUIRE(m.Element[1][1] == 4.0f);
    }
}
