#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "xe/math/Plane.h"

TEST_CASE("plane default constructor makes the XZ plane pointing +Y", "[plane]") {
    const xe::plane subject;
    REQUIRE(subject.a == 0.0f);
    REQUIRE(subject.b == 1.0f);
    REQUIRE(subject.c == 0.0f);
    REQUIRE(subject.d == 0.0f);
}

TEST_CASE("plane four-param constructor", "[plane]") {
    const xe::plane p{1.0f, 2.0f, 3.0f, 4.0f};
    REQUIRE(p.a == 1.0f);
    REQUIRE(p.b == 2.0f);
    REQUIRE(p.c == 3.0f);
    REQUIRE(p.d == 4.0f);
}

TEST_CASE("plane::data points at the internal (a, b, c, d) buffer", "[plane][data]") {
    const xe::plane p{1.0f, 2.0f, 3.0f, 4.0f};
    const float *values = p.data();
    REQUIRE(values[0] == 1.0f);
    REQUIRE(values[1] == 2.0f);
    REQUIRE(values[2] == 3.0f);
    REQUIRE(values[3] == 4.0f);
}

TEST_CASE("plane equality is element-wise", "[plane][equality]") {
    const xe::plane a{1.0f, 2.0f, 3.0f, 4.0f};
    REQUIRE(a == xe::plane{1.0f, 2.0f, 3.0f, 4.0f});
    REQUIRE(a != xe::plane{-1.0f, 2.0f, 3.0f, 4.0f});
    REQUIRE(a != xe::plane{1.0f, -2.0f, 3.0f, 4.0f});
    REQUIRE(a != xe::plane{1.0f, 2.0f, -3.0f, 4.0f});
    REQUIRE(a != xe::plane{1.0f, 2.0f, 3.0f, -4.0f});
}

TEST_CASE("planeVectorial builds a plane from a normal and a point", "[plane][planeVectorial]") {
    const auto p = xe::planeVectorial(xe::vec3{1.0f, 0.0f, 0.0f}, xe::vec3{1.0f, 1.0f, 1.0f});
    REQUIRE(p == xe::plane{1.0f, 0.0f, 0.0f, 1.0f});
    REQUIRE(xe::length2(p.normal()) == 1.0f);
}

TEST_CASE("plane::evaluate returns the signed distance", "[plane][evaluate]") {
    const xe::plane p{-1.0f, 2.0f, -3.0f, 1.0f};
    REQUIRE(p.evaluate({0.0f, 0.0f, 0.0f}) == -1.0f);
    REQUIRE(p.evaluate({1.0f, 1.0f, 1.0f}) == -3.0f);
    REQUIRE(p.evaluate({-1.0f, 0.0f, 0.0f}) == 0.0f);
    REQUIRE(p.evaluate({-1.0f, 1.0f, 0.0f}) == 2.0f);
}

TEST_CASE("plane::intersect on the three axis planes", "[plane][intersect]") {
    const xe::plane px{1.0f, 0.0f, 0.0f, 1.0f};
    const xe::plane py{0.0f, 1.0f, 0.0f, 1.0f};
    const xe::plane pz{0.0f, 0.0f, 1.0f, 1.0f};
    REQUIRE(px.intersect(py));
    REQUIRE(py.intersect(pz));
    REQUIRE(pz.intersect(px));
}

TEST_CASE("plane::test classifies a point against the plane", "[plane][test]") {
    const xe::plane p{0.0f, 1.0f, 0.0f, 1.0f};
    REQUIRE(p.test({0.0f, 2.0f, 0.0f}) == xe::PlaneSide::Front);
    REQUIRE(p.test({0.0f, -2.0f, 0.0f}) == xe::PlaneSide::Back);
    REQUIRE(p.test({0.0f, 1.0f, 0.0f}) == xe::PlaneSide::Inside);
}

TEST_CASE("plane serialization produces a non-empty string", "[plane][serialization]") {
    std::stringstream ss;
    ss << xe::plane{};
    REQUIRE_FALSE(ss.str().empty());

    std::stringstream xy;
    xy << xe::planeXY();
    std::stringstream yz;
    yz << xe::planeYZ();
    REQUIRE(xy.str() != yz.str());
}
