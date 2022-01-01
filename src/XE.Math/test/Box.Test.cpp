
#include <XE/Math/Box.h>
#include <catch2/catch_all.hpp>
#include <sstream>

TEST_CASE("Box's can be created using common constructors, with a valid state", "[Box]") {
    SECTION("default constructor should have zero-vector in its edges") {
        XE::Boxf box{};

        REQUIRE(box.getMinEdge() == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(box.isValid());
    }

    SECTION("Boundary(const Vector&) should set both edges with the supplied parameter ") {
        XE::Boxf box{{1.0f, 2.0f, 3.0f}};

        REQUIRE(box.getMinEdge() == XE::Vector3f{1.0f, 2.0f, 3.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{1.0f, 2.0f, 3.0f});
        REQUIRE(box.isValid());
    }

    SECTION("Boundary(const Vector&, const Vector&) should let the box with a Valid state") {
        XE::Boxf box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

        REQUIRE(box.getMinEdge() == XE::Vector3f{-1.0f, 2.0f, -3.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{1.0f, 8.0f, 3.0f});
        REQUIRE(box.isValid());
    }
}


TEST_CASE("Box's attributes change based on values supplied via constructors and mutator methods", "[Box]") {
    XE::Boxf box {{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};
    
    SECTION("getSize() should return a vector with the computed size") {
        REQUIRE(box.getSize() == XE::Vector3f{2.0f, 6.0f, 6.0f});
    }

    SECTION("getCenter() should return a vector with the computed center of the Box") {
        REQUIRE(box.getCenter() == XE::Vector3f{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should not mutate the state when using it with a point inside the Box") {
        box.expand(box.getCenter());

        REQUIRE(box.getSize() == XE::Vector3f{2.0f, 6.0f, 6.0f});
        REQUIRE(box.getCenter() == XE::Vector3f{0.0f, 5.0f, 0.0f});
    }
}


TEST_CASE("Box's can be serialized to an ostream", "[Box]") {
    XE::Boxf box {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    SECTION("operator<<() should create an unique string representation") {
        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box;
        REQUIRE(ss.str() == ss1.str());
    }

    SECTION("operator<<() should create an unique string representation from different boxes") {
        XE::Boxf box1 {{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}};

        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box1;
        REQUIRE(ss.str() != ss1.str());
    }
}
