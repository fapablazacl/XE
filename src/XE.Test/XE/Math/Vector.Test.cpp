
#include <catch.hpp>
#include <XE/Math/Vector.hpp>

using namespace XE;
using namespace XE::Math;

TEST_CASE("XE::Math::Vector<3, float>") {
    SECTION("constructor for 3 components should set the parameters correctly") {
        const Vector3f v = {1.0f, 2.0f, 3.0f};

        REQUIRE(v.X == 1.0f);
        REQUIRE(v.Y == 2.0f);
        REQUIRE(v.Z == 3.0f);

        REQUIRE(v.Data[0] == 1.0f);
        REQUIRE(v.Data[1] == 2.0f);
        REQUIRE(v.Data[2] == 3.0f);

        REQUIRE(v[0] == 1.0f);
        REQUIRE(v[1] == 2.0f);
        REQUIRE(v[2] == 3.0f);
    }

    SECTION("Comparison operators should operate correctly") {
        const Vector3f v1 = {1.0f, 2.0f, 3.0f};
        const Vector3f v2 = {1.0f, 2.0f, 3.0f};
        const Vector3f v3 = {-1.0f, -2.0f, -3.0f};

        REQUIRE(v1 == v2);
        REQUIRE(v1 != v3);
        REQUIRE(v2 != v3);
    }

    SECTION("operator+ should add component-wise") {
        const Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v1 + v2) == Vector3f{-1.0f, -3.0f, 1.0f});

        REQUIRE(v1 == +v1);
        REQUIRE(v2 == +v2);

        REQUIRE((Vector3f{0.0f} += v1) == v1);
        REQUIRE((Vector3f{0.0f} += v2) == v2);
    }

    SECTION("operator- should subtract component-wise") {
        const Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v1 - v2) == Vector3f{3.0f, -1.0f, 5.0f});
        REQUIRE((v1 - v1) == Vector3f{0.0f});
        REQUIRE((v2 - v2) == Vector3f{0.0f});

        REQUIRE(Vector3f{-1.0f, 2.0f, -3.0f} == -v1);
        REQUIRE(Vector3f{2.0f, 1.0f, 2.0f} == -v2);

        REQUIRE((Vector3f{0.0f} -= v1) == -v1);
        REQUIRE((Vector3f{0.0f} -= v2) == -v2);
    }
    
    SECTION("operator* should multiply component-wise") {
        const Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v1 * v2) == Vector3f{-2.0f, 2.0f, -6.0f});
        REQUIRE((v1 * 1.0f) == v1);
        REQUIRE((v1 * -1.0f) == -v1);
        REQUIRE((v1 * 2.0f) == Vector3f{2.0f, -4.0f, 6.0f});
        REQUIRE((v2 * 1.0f) == v2);
        REQUIRE((v2 * -1.0f) == -v2);
        REQUIRE((v2 * 2.0f) == Vector3f{-4.0f, -2.0f, -4.0f});

        REQUIRE((1.0f * v2) == v2);
        REQUIRE((-1.0f * v2) == -v2);
        REQUIRE((1.0f * v1) == v1);
        REQUIRE((-1.0f * v1) == -v1);

        REQUIRE((1.4f * v1) == (v1 * 1.4f));
        REQUIRE((1.4f * v2) == (v2 * 1.4f));

        REQUIRE((Vector3f{1.0f} *= v1) == v1);
        REQUIRE((Vector3f{1.0f} *= v2) == v2);
    }

    SECTION("operator/ should divide component-wise") {
        const Vector3f v1 = {2.0f, 8.0f, 32.0f};
        const Vector3f v2 = {1.0f, 2.0f, 4.0f};

        REQUIRE((v1 / v2) == Vector3f{2.0f, 4.0f, 8.0f});
        REQUIRE((v1 / 2.0f) == Vector3f{1.0f, 4.0f, 16.0f});
        REQUIRE((v2 / 2.0f) == Vector3f{0.5f, 1.0f, 2.0f});

        REQUIRE((Vector3f{v1} /= v1) == Vector3f{1.0f, 1.0f, 1.0f});
        REQUIRE((Vector3f{v2} /= v2) == Vector3f{1.0f, 1.0f, 1.0f});
    }

    SECTION("Dot Product should perform correctly") {
        const Vector3f v1 = {2.0f, 8.0f, 32.0f};
        const Vector3f v2 = {1.0f, 2.0f, 4.0f};

        REQUIRE(Dot(Vector3f{1.0f, 1.0f, 1.0f}, v1) == 42.0f);
        REQUIRE(Dot(Vector3f{1.0f, 1.0f, 1.0f}, v2) == 7.0f);
        REQUIRE(Dot(Vector3f{0.0f}, v1) == 0.0f);
        REQUIRE(Dot(Vector3f{0.0f}, v2) == 0.0f);

        REQUIRE(Dot(v1, v2) == 146.0f);
    }

    SECTION("Dot Product should perform correctly") {
        const Vector3f v1 = {2.0f, 8.0f, 32.0f};
        const Vector3f v2 = {1.0f, 2.0f, 4.0f};

        REQUIRE(Dot(Vector3f{1.0f, 1.0f, 1.0f}, v1) == 42.0f);
        REQUIRE(Dot(Vector3f{1.0f, 1.0f, 1.0f}, v2) == 7.0f);
        REQUIRE(Dot(Vector3f{0.0f}, v1) == 0.0f);
        REQUIRE(Dot(Vector3f{0.0f}, v2) == 0.0f);

        REQUIRE(Dot(v1, v2) == 146.0f);
    }

    SECTION("Cross Product should perform correctly") {
        const Vector3f v1 = {1.0f, 0.0f, 0.0f};
        const Vector3f v2 = {0.0f, 1.0f, 0.0f};
        const Vector3f v3 = {0.0f, 0.0f, 1.0f};

        REQUIRE(Cross(v1, v2) == v3);
        REQUIRE(Cross(v2, v1) == -v3);

        REQUIRE(Cross(v2, v3) == v1);
        REQUIRE(Cross(v3, v2) == -v1);

        REQUIRE(Cross(v3, v1) == v2);
        REQUIRE(Cross(v1, v3) == -v2);

        REQUIRE(Cross(v1, v1) == Vector3f{0.0f});
        REQUIRE(Cross(v2, v2) == Vector3f{0.0f});
        REQUIRE(Cross(v3, v3) == Vector3f{0.0f});
    }
}
