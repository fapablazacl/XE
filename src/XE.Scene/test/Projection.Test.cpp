
#include <catch2/catch_all.hpp>

#include <XE/Math/Common.h>
#include <XE/Scene/Projection.h>

TEST_CASE("XE::Graphics::project function", "[Projection]") {
    SECTION("When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)") {
        const XE::Matrix4f projViewModel = XE::Matrix4f::identity();
        const XE::Viewport viewport = { {0, 0}, {640, 480} };

        SECTION("Points placed into the XY plane at Z = 0 should map directly inside the Viewport") {
            REQUIRE(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 240.0f, 0.0f, 1.0f});
            REQUIRE(XE::project({-1.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{0.0f, 240.0f, 0.0f, 1.0f});
            REQUIRE(XE::project({1.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{640.0f, 240.0f, 0.0f, 1.0f});
            REQUIRE(XE::project({0.0f, 1.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 480.0f, 0.0f, 1.0f});
            REQUIRE(XE::project({0.0f, -1.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 0.0f, 0.0f, 1.0f});
        }

        SECTION("and also At Z=0.5") {
            REQUIRE(XE::project({0.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 240.0f, 0.5f, 1.0f});
            REQUIRE(XE::project({-1.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport) == XE::Vector4f{0.0f, 240.0f, 0.5f, 1.0f});
            REQUIRE(XE::project({1.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport) == XE::Vector4f{640.0f, 240.0f, 0.5f, 1.0f});
            REQUIRE(XE::project({0.0f, 1.0f, 0.5f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 480.0f, 0.5f, 1.0f});
            REQUIRE(XE::project({0.0f, -1.0f, 0.5f, 1.0f}, projViewModel, viewport) == XE::Vector4f{320.0f, 0.0f, 0.5f, 1.0f});
        }

        SECTION("Points placed outside the [-1.0, 1.0]^2 range map outside the viewport") {
            REQUIRE(XE::project({-2.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{-320.0f, 240.0f, 0.0f, 1.0f});
            REQUIRE(XE::project({-2.0f, 2.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{-320.0f, 720.0f, 0.0f, 1.0f});
        }

        SECTION("Vectors (with W = 0) should keep pointing in the same direction") {
            const auto projected = XE::project({1.0f, 1.0f, 0.0f, 0.0f}, projViewModel, viewport);
            REQUIRE(projected == XE::Vector4f{640.0f, 480.0f, 0.0f, 0.0f});
        }
    }

    SECTION("When consideraring a perspective transformation pipeline, with a viewport of (0, 0) - (640, 480)") {
        const XE::Matrix4f projViewModel = 
            XE::Matrix4f::perspective(XE::radians(90.0f), 640.0f/480.0f, 0.1f, 100.0f) * 
            XE::Matrix4f::lookAtRH({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
            
        const XE::Viewport viewport = { {0, 0}, {640, 480} };
        REQUIRE(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport).X == 320.0f);
        REQUIRE(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport).Y == 240.0f);
    }
}


TEST_CASE("XE::Graphics::unproject function") {
    SECTION("When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)") {
        const XE::Matrix4f projViewModel = XE::Matrix4f::identity();
        const XE::Viewport viewport = { {0, 0}, {640, 480} };

        SECTION("Points placed inside the screen are in the [-1, 1]^2 domain") {
            REQUIRE(XE::unproject({320.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
            REQUIRE(XE::unproject({0.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{-1.0f, 0.0f, 0.0f, 1.0f});
            REQUIRE(XE::unproject({640.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{1.0f, 0.0f, 0.0f, 1.0f});
            REQUIRE(XE::unproject({320.0f, 480.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{0.0f, 1.0f, 0.0f, 1.0f});
            REQUIRE(XE::unproject({320.0f, 0.0f, 0.0f, 1.0f} , projViewModel, viewport) == XE::Vector4f{0.0f, -1.0f, 0.0f, 1.0f});
        }

        SECTION("Points placed outside the screen are out of the [-1.0, 1.0]^2") {
            REQUIRE(XE::unproject({-320.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{-2.0f, 0.0f, 0.0f, 1.0f});
            REQUIRE(XE::unproject({-320.0f, 720.0f, 0.0f, 1.0f}, projViewModel, viewport) == XE::Vector4f{-2.0f, 2.0f, 0.0f, 1.0f});
        }
    }
}
