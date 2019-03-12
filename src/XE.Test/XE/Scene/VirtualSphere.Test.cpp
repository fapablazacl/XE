
#include <catch.hpp>
#include <XE/Scene/VirtualSphere.hpp>

TEST_CASE("VirtualSphere accessors should full its fields properly", "[VirtualSphere]") {
    SECTION("Given virtual spheres with standard sizes") {
        auto vs1 = XE::VirtualSphere{{640, 480}};
        auto vs2 = XE::VirtualSphere{{800, 600}};

        SECTION("Their sizes should match with those supplied in the constructor") {
            vs1.setScreenSize({1024, 768});
            vs2.setScreenSize({1360, 1024});

            REQUIRE(vs1.getScreenSize() == XE::Vector2f{1024, 768});
            REQUIRE(vs2.getScreenSize() == XE::Vector2f{1360, 1024});
        }

        SECTION("Their sizes should match with those supplied in the setters") {
            REQUIRE(vs1.getScreenSize() == XE::Vector2f{640, 480});
            REQUIRE(vs2.getScreenSize() == XE::Vector2f{800, 600});
        }
    }
}

TEST_CASE("VirtualSphere computePointAt should return points lying inside the surface of the sphere", "[VirtualSphere]") {
    SECTION("Given virtual spheres with standard sizes") {
        auto vs1 = XE::VirtualSphere{{640, 480}};
        auto vs2 = XE::VirtualSphere{{800, 600}};

        XE::VirtualSphere spheres[] = {
            {{100, 100}}, 
            {{320, 200}},
            {{200, 320}},
        };

        for (XE::VirtualSphere sphere : spheres) {
            REQUIRE(sphere.computePointAt({0, 0}) == XE::Vector3f{-0.707107, -0.707107, 0});
        }
    }
}
