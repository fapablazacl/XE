

#include "xe/graphics/Viewport.h"
#include "xe/math/Matrix.h"
#include <gtest/gtest.h>

#include <xe/math/Common.h>
#include <xe/scene/Projection.h>

TEST(ProjectionTest, ProjectFunction) {
    // When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)
    const XE::Matrix4 projViewModel = XE::mat4Identity();
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    // SECTION("Points placed into the XY plane at Z = 0 should map directly inside the Viewport") {
    EXPECT_EQ(XE::project({0.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 240.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::project({-1.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(0.0F, 240.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::project({1.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(640.0F, 240.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::project({0.0F, 1.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 480.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::project({0.0F, -1.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 0.0F, 0.0F, 1.0F));

    // SECTION("and also At Z=0.5") {
    EXPECT_EQ(XE::project({0.0F, 0.0F, 0.5F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 240.0F, 0.5F, 1.0F));
    EXPECT_EQ(XE::project({-1.0F, 0.0F, 0.5F, 1.0F}, projViewModel, viewport), XE::Vector4(0.0F, 240.0F, 0.5F, 1.0F));
    EXPECT_EQ(XE::project({1.0F, 0.0F, 0.5F, 1.0F}, projViewModel, viewport), XE::Vector4(640.0F, 240.0F, 0.5F, 1.0F));
    EXPECT_EQ(XE::project({0.0F, 1.0F, 0.5F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 480.0F, 0.5F, 1.0F));
    EXPECT_EQ(XE::project({0.0F, -1.0F, 0.5F, 1.0F}, projViewModel, viewport), XE::Vector4(320.0F, 0.0F, 0.5F, 1.0F));

    // SECTION("Points placed outside the [-1.0, 1.0]^2 range map outside the viewport") {
    EXPECT_EQ(XE::project({-2.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(-320.0F, 240.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::project({-2.0F, 2.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(-320.0F, 720.0F, 0.0F, 1.0F));

    // SECTION("Vectors (with W = 0) should keep pointing in the same direction") {
    const auto projected = XE::project({1.0F, 1.0F, 0.0F, 0.0F}, projViewModel, viewport);
    EXPECT_EQ(projected, XE::Vector4(640.0F, 480.0F, 0.0F, 0.0F));
}

TEST(ProjectionTest, ProjectFunctionPerpective) {
    const XE::Matrix4 proj = XE::mat4Perspective(XE::radians(90.0F), 640.0F / 480.0F, 0.1F, 100.0F);
    const XE::Matrix4 view = XE::mat4LookAtRH({0.0F, 0.0F, -1.0F}, {0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    const XE::Matrix4 projViewModel = proj * view;
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    EXPECT_EQ(XE::project({0.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport).X, 320.0F);
    EXPECT_EQ(XE::project({0.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport).Y, 240.0F);
}

TEST(ProjectionTest, UnprojectFunction) {
    // SECTION("When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)") {
    const XE::Matrix4 projViewModel = XE::mat4Identity();
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    // SECTION("Points placed inside the screen are in the [-1, 1]^2 domain") {
    EXPECT_EQ(XE::unproject({320.0F, 240.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::unproject({0.0F, 240.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(-1.0F, 0.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::unproject({640.0F, 240.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(1.0F, 0.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::unproject({320.0F, 480.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(0.0F, 1.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::unproject({320.0F, 0.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(0.0F, -1.0F, 0.0F, 1.0F));

    // SECTION("Points placed outside the screen are out of the [-1.0, 1.0]^2") {
    EXPECT_EQ(XE::unproject({-320.0F, 240.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(-2.0F, 0.0F, 0.0F, 1.0F));
    EXPECT_EQ(XE::unproject({-320.0F, 720.0F, 0.0F, 1.0F}, projViewModel, viewport), XE::Vector4(-2.0F, 2.0F, 0.0F, 1.0F));
}
