#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Vector.h"

using Catch::Matchers::WithinRel;

TEST_CASE("vec3 default constructor initializes to zero", "[vector][vec3]") {
    const xe::vec3 v;
    REQUIRE(v.x == 0.0f);
    REQUIRE(v.y == 0.0f);
    REQUIRE(v.z == 0.0f);
}

TEST_CASE("vec3 component-wise constructor", "[vector][vec3]") {
    const xe::vec3 v{1.0f, 2.0f, 3.0f};
    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);
    REQUIRE(v[0] == 1.0f);
    REQUIRE(v[1] == 2.0f);
    REQUIRE(v[2] == 3.0f);
}

TEST_CASE("vec3 scalar splat constructor", "[vector][vec3]") {
    const xe::vec3 v(5.0f);
    REQUIRE(v.x == 5.0f);
    REQUIRE(v.y == 5.0f);
    REQUIRE(v.z == 5.0f);
}

TEST_CASE("vec3 from pointer", "[vector][vec3]") {
    const float data[] = {1.0f, 2.0f, 3.0f};
    const xe::vec3 v(data);
    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);
}

TEST_CASE("vec3 from vec2 + scalar", "[vector][vec3]") {
    const xe::vec2 a{1.0f, 2.0f};
    const xe::vec3 v{a, 3.0f};
    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);
}

TEST_CASE("vec4 from vec3 + scalar", "[vector][vec4]") {
    const xe::vec3 a{1.0f, 2.0f, 3.0f};
    const xe::vec4 v{a, 4.0f};
    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);
    REQUIRE(v.w == 4.0f);
}

TEST_CASE("data() returns a pointer to the first component", "[vector][vec3]") {
    xe::vec3 v{1.0f, 2.0f, 3.0f};
    REQUIRE(v.data() != nullptr);
    REQUIRE(v.data()[0] == 1.0f);
    REQUIRE(v.data()[1] == 2.0f);
    REQUIRE(v.data()[2] == 3.0f);
}

TEST_CASE("vector arithmetic operators", "[vector][vec3]") {
    const xe::vec3 a{1.0f, 2.0f, 3.0f};
    const xe::vec3 b{4.0f, 5.0f, 6.0f};

    SECTION("addition") {
        const auto r = a + b;
        REQUIRE(r == xe::vec3{5.0f, 7.0f, 9.0f});
    }
    SECTION("subtraction") {
        const auto r = b - a;
        REQUIRE(r == xe::vec3{3.0f, 3.0f, 3.0f});
    }
    SECTION("componentwise multiplication") {
        const auto r = a * b;
        REQUIRE(r == xe::vec3{4.0f, 10.0f, 18.0f});
    }
    SECTION("componentwise division") {
        const auto r = b / a;
        REQUIRE(r == xe::vec3{4.0f, 2.5f, 2.0f});
    }
    SECTION("scalar multiplication (both orders)") {
        REQUIRE(a * 2.0f == xe::vec3{2.0f, 4.0f, 6.0f});
        REQUIRE(2.0f * a == xe::vec3{2.0f, 4.0f, 6.0f});
    }
    SECTION("unary negation") {
        REQUIRE(-a == xe::vec3{-1.0f, -2.0f, -3.0f});
    }
}

TEST_CASE("compound assignment operators", "[vector][vec3]") {
    xe::vec3 a{1.0f, 2.0f, 3.0f};
    const xe::vec3 b{4.0f, 5.0f, 6.0f};

    a += b;
    REQUIRE(a == xe::vec3{5.0f, 7.0f, 9.0f});

    a -= b;
    REQUIRE(a == xe::vec3{1.0f, 2.0f, 3.0f});

    a *= 2.0f;
    REQUIRE(a == xe::vec3{2.0f, 4.0f, 6.0f});

    a /= 2.0f;
    REQUIRE(a == xe::vec3{1.0f, 2.0f, 3.0f});
}

TEST_CASE("dot product", "[vector][dot]") {
    const xe::vec3 a{1.0f, 2.0f, 3.0f};
    const xe::vec3 b{4.0f, -5.0f, 6.0f};
    REQUIRE(xe::dot(a, b) == 1.0f * 4.0f + 2.0f * -5.0f + 3.0f * 6.0f);
}

TEST_CASE("cross product follows the right-handed basis rule", "[vector][cross]") {
    const xe::vec3 x{1.0f, 0.0f, 0.0f};
    const xe::vec3 y{0.0f, 1.0f, 0.0f};
    const xe::vec3 z{0.0f, 0.0f, 1.0f};
    REQUIRE(xe::cross(x, y) == z);
    REQUIRE(xe::cross(y, z) == x);
    REQUIRE(xe::cross(z, x) == y);
}

TEST_CASE("length and length2", "[vector][length]") {
    const xe::vec3 v{3.0f, 4.0f, 0.0f};
    REQUIRE(xe::length2(v) == 25.0f);
    REQUIRE_THAT(xe::length(v), WithinRel(5.0f, 1e-6f));
}

TEST_CASE("normalize produces a unit vector", "[vector][normalize]") {
    const xe::vec3 v{3.0f, 0.0f, 4.0f};
    const auto n = xe::normalize(v);
    REQUIRE_THAT(xe::length(n), WithinRel(1.0f, 1e-6f));
    REQUIRE_THAT(n.x, WithinRel(0.6f, 1e-6f));
    REQUIRE_THAT(n.z, WithinRel(0.8f, 1e-6f));
}

TEST_CASE("min and max are componentwise", "[vector][min][max]") {
    const xe::vec3 a{1.0f, 5.0f, 3.0f};
    const xe::vec3 b{4.0f, 2.0f, 6.0f};
    REQUIRE(xe::min(a, b) == xe::vec3{1.0f, 2.0f, 3.0f});
    REQUIRE(xe::max(a, b) == xe::vec3{4.0f, 5.0f, 6.0f});
}

TEST_CASE("vec3 is constexpr-constructible", "[vector][constexpr]") {
    constexpr xe::vec3 v{1.0f, 2.0f, 3.0f};
    static_assert(v.x == 1.0f);
    static_assert(v.y == 2.0f);
    static_assert(v.z == 3.0f);
    // xe::dot uses operator[] which is not strictly constexpr under the
    // union-based layout trick — we cross-check the value at runtime instead.
    REQUIRE(xe::dot(v, v) == 14.0f);
}

TEST_CASE("cross-type vector construction", "[vector][conversion]") {
    const xe::ivec3 vi{1, 2, 3};
    const xe::vec3 vf{vi};
    REQUIRE(vf.x == 1.0f);
    REQUIRE(vf.y == 2.0f);
    REQUIRE(vf.z == 3.0f);
}

TEST_CASE("legacy uppercase X/Y/Z members still work", "[vector][legacy]") {
    const xe::vec3 v{1.0f, 2.0f, 3.0f};
    REQUIRE(v.X == 1.0f);
    REQUIRE(v.Y == 2.0f);
    REQUIRE(v.Z == 3.0f);
}

TEST_CASE("legacy norm/norm2 forwarders", "[vector][legacy]") {
    const xe::vec3 v{3.0f, 4.0f, 0.0f};
    REQUIRE(xe::norm2(v) == 25.0f);
    REQUIRE_THAT(xe::norm(v), WithinRel(5.0f, 1e-6f));
}
