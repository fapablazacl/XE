#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Matrix.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("mat4 default constructor yields zero matrix", "[matrix][mat4]") {
    const xe::mat4 m;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            REQUIRE(m(r, c) == 0.0f);
        }
    }
}

TEST_CASE("mat4(1.0f) is the identity matrix", "[matrix][identity]") {
    const xe::mat4 m(1.0f);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            REQUIRE(m(r, c) == (r == c ? 1.0f : 0.0f));
        }
    }
}

TEST_CASE("mat4 column access via operator[]", "[matrix][access]") {
    xe::mat4 m(1.0f);
    m(0, 3) = 5.0f; // translation X
    m(1, 3) = 7.0f;

    SECTION("operator[] returns the column") {
        REQUIRE(m[3].x == 5.0f);
        REQUIRE(m[3].y == 7.0f);
        REQUIRE(m[3].z == 0.0f);
        REQUIRE(m[3].w == 1.0f);
    }

    SECTION("operator()(row, col) agrees with column access") {
        REQUIRE(m(0, 3) == m[3].x);
        REQUIRE(m(1, 3) == m[3].y);
    }
}

TEST_CASE("mat4 * mat4 identity is idempotent", "[matrix][multiplication]") {
    const xe::mat4 I(1.0f);
    const auto product = I * I;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            REQUIRE(product(r, c) == (r == c ? 1.0f : 0.0f));
        }
    }
}

TEST_CASE("mat4 * vec4 with identity is the vector", "[matrix][mul]") {
    const xe::mat4 I(1.0f);
    const xe::vec4 v{1.0f, 2.0f, 3.0f, 1.0f};
    const auto r = I * v;
    REQUIRE(r == v);
}

TEST_CASE("xe::translate moves a point", "[matrix][translate]") {
    const auto t = xe::translate(xe::vec3{5.0f, 6.0f, 7.0f});
    const xe::vec4 origin{0.0f, 0.0f, 0.0f, 1.0f};
    const auto moved = t * origin;
    REQUIRE(moved.x == 5.0f);
    REQUIRE(moved.y == 6.0f);
    REQUIRE(moved.z == 7.0f);
    REQUIRE(moved.w == 1.0f);
}

TEST_CASE("xe::scale scales a point", "[matrix][scale]") {
    const auto s = xe::scale(xe::vec3{2.0f, 3.0f, 4.0f});
    const xe::vec4 p{1.0f, 1.0f, 1.0f, 1.0f};
    const auto scaled = s * p;
    REQUIRE(scaled.x == 2.0f);
    REQUIRE(scaled.y == 3.0f);
    REQUIRE(scaled.z == 4.0f);
}

TEST_CASE("xe::rotateZ by 90 degrees maps +X to +Y", "[matrix][rotate]") {
    const auto r = xe::rotateZ(xe::radians(90.0f));
    const xe::vec4 px{1.0f, 0.0f, 0.0f, 1.0f};
    const auto rotated = r * px;
    REQUIRE_THAT(rotated.x, WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(rotated.y, WithinRel(1.0f, 1e-5f));
}

TEST_CASE("xe::transpose of a mat4", "[matrix][transpose]") {
    xe::mat4 m(1.0f);
    m(0, 1) = 2.0f;
    m(1, 0) = 3.0f;
    const auto t = xe::transpose(m);
    REQUIRE(t(0, 1) == 3.0f);
    REQUIRE(t(1, 0) == 2.0f);
    REQUIRE(t(0, 0) == 1.0f);
}

TEST_CASE("xe::determinant", "[matrix][determinant]") {
    SECTION("2x2") {
        xe::mat2 m;
        m(0, 0) = 4.0f;
        m(0, 1) = 2.0f;
        m(1, 0) = 3.0f;
        m(1, 1) = 1.0f;
        // det = 4*1 - 2*3 = -2
        REQUIRE(xe::determinant(m) == -2.0f);
    }

    SECTION("identity mat4 determinant is 1") {
        REQUIRE(xe::determinant(xe::mat4(1.0f)) == 1.0f);
    }
}

TEST_CASE("xe::inverse of identity is identity", "[matrix][inverse]") {
    const xe::mat4 I(1.0f);
    const auto inv = xe::inverse(I);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            REQUIRE_THAT(inv(r, c), WithinAbs(I(r, c), 1e-6f));
        }
    }
}

TEST_CASE("legacy Matrix4 and mat4Identity() still work", "[matrix][legacy]") {
    const xe::Matrix4 m = xe::mat4Identity();
    REQUIRE(m(0, 0) == 1.0f);
    REQUIRE(m(3, 3) == 1.0f);
    REQUIRE(m(0, 1) == 0.0f);
}

TEST_CASE("legacy mat4Perspective / mat4LookAtRH build without error", "[matrix][legacy]") {
    const auto proj = xe::mat4Perspective(xe::radians(60.0f), 1.0f, 0.1f, 100.0f);
    const auto view = xe::mat4LookAtRH(xe::Vector3{0.0f, 0.0f, 5.0f}, xe::Vector3{0.0f, 0.0f, 0.0f}, xe::Vector3{0.0f, 1.0f, 0.0f});
    // Sanity: perspective's (2,3) term is non-zero under glm convention.
    REQUIRE(proj(2, 3) != 0.0f);
    // RH view looking down -Z at (0,0,0) from (0,0,5) places view(2,3) = dot(f, eye)
    // where f = (0,0,-1), eye = (0,0,5) -> dot = -5.
    REQUIRE_THAT(view(2, 3), WithinRel(-5.0f, 1e-5f));
}
