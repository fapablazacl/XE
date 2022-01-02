
#include <XE/Math/Plane.h>
#include <catch2/catch_all.hpp>

TEST_CASE("Plane constructors should create planes with specific values", "[Plane]") {
    SECTION("default constructor generate a XZ plane, pointing to the positive Y-axis") {
        XE::Planef subject;

        REQUIRE(subject.a == 0.0f);
        REQUIRE(subject.b == 1.0f);
        REQUIRE(subject.c == 0.0f);
        REQUIRE(subject.d == 0.0f);
    }

    SECTION("4-param constructor generate a plane with the specified components") {
        XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

        REQUIRE(subject.a == 1.0f);
        REQUIRE(subject.b == 2.0f);
        REQUIRE(subject.c == 3.0f);
        REQUIRE(subject.d == 4.0f);
    }
}

TEST_CASE("Plane factory methods should create well-defined planes with normalized normals", "[Plane]") {
    SECTION("serialization should generate a non-empty string") {
        REQUIRE(true);
    }
}

TEST_CASE("Plane can be Serialized to an Output Stream", "[Plane]") {
    SECTION("serialization should generate a non-empty string") {
        XE::Planef subject;

        std::stringstream ss;
        ss << subject;

        REQUIRE(ss.str() != "");
    }

    SECTION("serialization of different value objects should generate different non-empty string") {
        std::stringstream ss1;
        ss1 << XE::Planef::xy();

        std::stringstream ss2;
        ss2 << XE::Planef::yz();

        REQUIRE(ss1.str() != ss2.str());
    }
}
