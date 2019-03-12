
#include <catch.hpp>
#include <XE/Scene/Trackball.hpp>

TEST_CASE("XE::Trackball class", "[Trackball]") {
    SECTION("should fill its fields properly") {
        REQUIRE(XE::Trackball{{640, 480}}.getSize() == XE::Vector2i{640, 480});
        REQUIRE(XE::Trackball{{800, 600}}.getSize() == XE::Vector2i{800, 600});

        auto trackball = XE::Trackball{{800, 600}};

        trackball.beginDrag({0, 0});
        trackball.drag({50, 50});
        trackball.endDrag({100, 100});

        REQUIRE(trackball.getDragBegin() == XE::Vector2i{0, 0});
        REQUIRE(trackball.getDragCurrent() == XE::Vector2i{50, 50});
        REQUIRE(trackball.getDragEnd() == XE::Vector2i{100, 100});
    }

    SECTION("should generate an empty rotation") {
        XE::Trackball trackball = {{640, 480}};

        trackball.beginDrag({100, 100});
        trackball.endDrag({100, 100});

        XE::Rotation rotation = trackball.computeRotation();

        REQUIRE(rotation.angle == 0.0);
    }

    SECTION("should generate a rotation around the Y axis with a horizontal mouse movement, inside the virtual unit trackball") {
        auto trackball = XE::Trackball{{100, 100}};

        trackball.beginDrag({50, 50});
        trackball.drag({55, 50});
        trackball.endDrag({60, 50});

        auto rotation = trackball.computeRotation();

        REQUIRE(rotation.axis == XE::Vector3f{0.0f, 1.0f, 0.0f});
        REQUIRE(rotation.angle > 0.0f);
    }

    /*
    SECTION("should generate a rotation around the X axis with a horizontal mouse movement, inside the virtual unit trackball") {
        auto trackball = XE::Trackball{{100, 100}};

        trackball.beginDrag({50, 50});
        trackball.drag({50, 55});
        trackball.endDrag({50, 60});

        auto rotation = trackball.computeRotation();

        REQUIRE(rotation.axis == XE::Vector3f{1.0f, 0.0f, 0.0f});
        REQUIRE(rotation.angle > 0.0f);
    }
    */
}
