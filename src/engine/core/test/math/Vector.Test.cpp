
#include "Common.h"

#include <sstream>
#include <xe/math/FormatUtils.h>
#include <xe/math/Vector.h>

/*
TEST_CASE("XE::Vector<3, float>", "[Vector]") {
    SECTION("constructor should setup the vector components correctly") {
        const XE::Vector3f v = {1.0f, 2.0f, 3.0f};

        REQUIRE(v.X == 1.0f);
        REQUIRE(v.Y == 2.0f);
        REQUIRE(v.Z == 3.0f);

        REQUIRE(v.values[0] == 1.0f);
        REQUIRE(v.values[1] == 2.0f);
        REQUIRE(v.values[2] == 3.0f);

        REQUIRE(v[0] == 1.0f);
        REQUIRE(v[1] == 2.0f);
        REQUIRE(v[2] == 3.0f);
    }

    SECTION("Comparison operators should check vector components for equality and inequality") {
        const XE::Vector3f v1 = {1.0f, 2.0f, 3.0f};
        const XE::Vector3f v2 = {1.0f, 2.0f, 3.0f};
        const XE::Vector3f v3 = {-1.0f, -2.0f, -3.0f};

        REQUIRE(v1 == v2);
        REQUIRE(v1 != v3);
        REQUIRE(v2 != v3);

        REQUIRE(v2 == v1);
        REQUIRE(v3 != v1);
        REQUIRE(v3 != v2);
    }

    SECTION("operator+ should add component-wise.") {
        const XE::Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const XE::Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v2 + v1) == XE::Vector3f{-1.0f, -3.0f, 1.0f});
        REQUIRE((v1 + v2) == XE::Vector3f{-1.0f, -3.0f, 1.0f});
        REQUIRE(v1 == +v1);
        REQUIRE(v2 == +v2);
        REQUIRE((XE::Vector3f{0.0f} += v1) == v1);
        REQUIRE((XE::Vector3f{0.0f} += v2) == v2);
    }

    SECTION("operator- should subtract component-wise the vector") {
        const XE::Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const XE::Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v2 - v1) == XE::Vector3f{-3.0f, 1.0f, -5.0f});
        REQUIRE((v1 - v2) == XE::Vector3f{3.0f, -1.0f, 5.0f});
        REQUIRE((v1 - v1) == XE::Vector3f{0.0f});
        REQUIRE((v2 - v2) == XE::Vector3f{0.0f});

        REQUIRE(XE::Vector3f{-1.0f, 2.0f, -3.0f} == -v1);
        REQUIRE(XE::Vector3f{2.0f, 1.0f, 2.0f} == -v2);

        REQUIRE((XE::Vector3f{0.0f} -= v1) == -v1);
        REQUIRE((XE::Vector3f{0.0f} -= v2) == -v2);
    }

    SECTION("operator* should multiply component-wise") {
        const XE::Vector3f v1 = {1.0f, -2.0f, 3.0f};
        const XE::Vector3f v2 = {-2.0f, -1.0f, -2.0f};

        REQUIRE((v1 * v2) == XE::Vector3f{-2.0f, 2.0f, -6.0f});
        REQUIRE((v2 * v1) == XE::Vector3f{-2.0f, 2.0f, -6.0f});

        REQUIRE((v1 * 1.0f) == v1);
        REQUIRE((v1 * -1.0f) == -v1);
        REQUIRE((v1 * 2.0f) == XE::Vector3f{2.0f, -4.0f, 6.0f});
        REQUIRE((v2 * 1.0f) == v2);
        REQUIRE((v2 * -1.0f) == -v2);
        REQUIRE((v2 * 2.0f) == XE::Vector3f{-4.0f, -2.0f, -4.0f});

        REQUIRE((1.0f * v2) == v2);
        REQUIRE((-1.0f * v2) == -v2);
        REQUIRE((1.0f * v1) == v1);
        REQUIRE((-1.0f * v1) == -v1);

        REQUIRE((1.4f * v1) == (v1 * 1.4f));
        REQUIRE((1.4f * v2) == (v2 * 1.4f));

        REQUIRE((XE::Vector3f{1.0f} *= v1) == v1);
        REQUIRE((XE::Vector3f{1.0f} *= v2) == v2);
    }

    SECTION("operator/ should divide component-wise") {
        const XE::Vector3f v1 = {2.0f, 8.0f, 32.0f};
        const XE::Vector3f v2 = {1.0f, 2.0f, 4.0f};

        REQUIRE((v1 / v2) == XE::Vector3f{2.0f, 4.0f, 8.0f});
        REQUIRE((v1 / 2.0f) == XE::Vector3f{1.0f, 4.0f, 16.0f});
        REQUIRE((v2 / 2.0f) == XE::Vector3f{0.5f, 1.0f, 2.0f});

        REQUIRE((XE::Vector3f{v1} /= v1) == XE::Vector3f{1.0f, 1.0f, 1.0f});
        REQUIRE((XE::Vector3f{v2} /= v2) == XE::Vector3f{1.0f, 1.0f, 1.0f});
    }

    SECTION("dot Product should perform correctly") {
        const XE::Vector3f v1 = {2.0f, 8.0f, 32.0f};
        const XE::Vector3f v2 = {1.0f, 2.0f, 4.0f};

        REQUIRE(dot(XE::Vector3f{1.0f, 1.0f, 1.0f}, v1) == 42.0f);
        REQUIRE(dot(XE::Vector3f{1.0f, 1.0f, 1.0f}, v2) == 7.0f);
        REQUIRE(dot(XE::Vector3f{0.0f}, v1) == 0.0f);
        REQUIRE(dot(XE::Vector3f{0.0f}, v2) == 0.0f);

        REQUIRE(dot(v1, XE::Vector3f{1.0f, 1.0f, 1.0f}) == 42.0f);
        REQUIRE(dot(v2, XE::Vector3f{1.0f, 1.0f, 1.0f}) == 7.0f);
        REQUIRE(dot(v1, XE::Vector3f{0.0f}) == 0.0f);
        REQUIRE(dot(v2, XE::Vector3f{0.0f}) == 0.0f);

        REQUIRE(dot(v1, v2) == 146.0f);
        REQUIRE(dot(v2, v1) == 146.0f);
    }

    SECTION("Three-Dimensional cross Product should perform correctly for unit vectors") {
        const XE::Vector3f v1 = {1.0f, 0.0f, 0.0f};
        const XE::Vector3f v2 = {0.0f, 1.0f, 0.0f};
        const XE::Vector3f v3 = {0.0f, 0.0f, 1.0f};

        REQUIRE(cross(v1, v2) == v3);
        REQUIRE(cross(v2, v1) == -v3);

        REQUIRE(cross(v2, v3) == v1);
        REQUIRE(cross(v3, v2) == -v1);

        REQUIRE(cross(v3, v1) == v2);
        REQUIRE(cross(v1, v3) == -v2);

        REQUIRE(cross(v1, v1) == XE::Vector3f{0.0f});
        REQUIRE(cross(v2, v2) == XE::Vector3f{0.0f});
        REQUIRE(cross(v3, v3) == XE::Vector3f{0.0f});
    }

    SECTION("minimize function should return the minimun values between two vectors") {
        const XE::Vector3f v1 = {1.0f, -2.0f, -1.3f};
        const XE::Vector3f v2 = {1.3f, -1.6f, 0.0f};
        const XE::Vector3f v3 = {-0.3f, 2.0f, 2.0f};

        REQUIRE(minimize(v1, v1) == v1);
        REQUIRE(minimize(v2, v2) == v2);
        REQUIRE(minimize(v3, v3) == v3);

        REQUIRE(minimize(v1, v2) == XE::Vector3f{1.0f, -2.0f, -1.3f});
        REQUIRE(minimize(v1, v3) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v2, v3) == XE::Vector3f{-0.3f, -1.6f, 0.0f});

        REQUIRE(minimize(v2, v1) == XE::Vector3f{1.0f, -2.0f, -1.3f});
        REQUIRE(minimize(v3, v1) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v3, v2) == XE::Vector3f{-0.3f, -1.6f, 0.0f});

        REQUIRE(minimize(v1, minimize(v2, v3)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v1, minimize(v3, v2)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v2, minimize(v1, v3)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v2, minimize(v3, v1)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v3, minimize(v2, v1)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
        REQUIRE(minimize(v3, minimize(v1, v2)) == XE::Vector3f{-0.3f, -2.0f, -1.3f});
    }

    SECTION("maximize function should return the minimun values between two vectors") {
        const XE::Vector3f v1 = {1.0f, -2.0f, -1.3f};
        const XE::Vector3f v2 = {1.3f, -1.6f, 0.0f};
        const XE::Vector3f v3 = {-0.3f, 2.0f, 2.0f};

        REQUIRE(maximize(v1, v1) == v1);
        REQUIRE(maximize(v2, v2) == v2);
        REQUIRE(maximize(v3, v3) == v3);

        REQUIRE(maximize(v1, v2) == XE::Vector3f{1.3f, -1.6f, 0.0f});
        REQUIRE(maximize(v1, v3) == XE::Vector3f{1.0f, 2.0f, 2.0f});
        REQUIRE(maximize(v2, v3) == XE::Vector3f{1.3f, 2.0f, 2.0f});

        REQUIRE(maximize(v2, v1) == XE::Vector3f{1.3f, -1.6f, 0.0f});
        REQUIRE(maximize(v3, v1) == XE::Vector3f{1.0f, 2.0f, 2.0f});
        REQUIRE(maximize(v3, v2) == XE::Vector3f{1.3f, 2.0f, 2.0f});

        REQUIRE(maximize(v1, maximize(v2, v3)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
        REQUIRE(maximize(v1, maximize(v3, v2)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
        REQUIRE(maximize(v2, maximize(v1, v3)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
        REQUIRE(maximize(v2, maximize(v3, v1)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
        REQUIRE(maximize(v3, maximize(v2, v1)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
        REQUIRE(maximize(v3, maximize(v1, v2)) == XE::Vector3f{1.3f, 2.0f, 2.0f});
    }

    SECTION("norm2 function should return an squared vector length") {
        const XE::Vector3f v1 = {1.0f, -2.0f, -1.6f};
        const XE::Vector3f v2 = {1.3f, -1.6f, 0.0f};
        const XE::Vector3f v3 = {-0.3f, 2.0f, 2.0f};

        REQUIRE(norm2(v1) == 1.0f + 4.0f + 1.6f*1.6f);
        REQUIRE(norm2(v2) == 1.3f*1.3f + 1.6f*1.6f);
        REQUIRE(norm2(v3) == 0.3f*0.3f + 4.0f + 4.0f);
    }

    SECTION("norm function should return the vector length") {
        const XE::Vector3f v1 = {0.0f, 0.0f, 0.0f};
        const XE::Vector3f v2 = {4.0f, 2.0f, 4.0f};
        const XE::Vector3f v3 = {1.0f, 1.0f, 1.0f};

        REQUIRE(norm(v1) == 0.0f);
        REQUIRE(norm(v2) == 6.0f);
        REQUIRE(norm(v3) == std::sqrt(3.0f));
    }
}
*/