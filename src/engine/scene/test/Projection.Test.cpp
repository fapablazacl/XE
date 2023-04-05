

#include <gtest/gtest.h>

#include <xe/math/Common.h>
#include <xe/scene/Projection.h>

TEST(ProjectionTest, ProjectFunction) {
    // When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)
    const XE::Matrix4 projViewModel = XE::Matrix4::identity();
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    // SECTION("Points placed into the XY plane at Z = 0 should map directly inside the Viewport") {
    EXPECT_EQ(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 240.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::project({-1.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(0.0f, 240.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::project({1.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(640.0f, 240.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::project({0.0f, 1.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 480.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::project({0.0f, -1.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 0.0f, 0.0f, 1.0f));

    // SECTION("and also At Z=0.5") {
    EXPECT_EQ(XE::project({0.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 240.0f, 0.5f, 1.0f));
    EXPECT_EQ(XE::project({-1.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport), XE::Vector4(0.0f, 240.0f, 0.5f, 1.0f));
    EXPECT_EQ(XE::project({1.0f, 0.0f, 0.5f, 1.0f}, projViewModel, viewport), XE::Vector4(640.0f, 240.0f, 0.5f, 1.0f));
    EXPECT_EQ(XE::project({0.0f, 1.0f, 0.5f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 480.0f, 0.5f, 1.0f));
    EXPECT_EQ(XE::project({0.0f, -1.0f, 0.5f, 1.0f}, projViewModel, viewport), XE::Vector4(320.0f, 0.0f, 0.5f, 1.0f));

    // SECTION("Points placed outside the [-1.0, 1.0]^2 range map outside the viewport") {
    EXPECT_EQ(XE::project({-2.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(-320.0f, 240.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::project({-2.0f, 2.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(-320.0f, 720.0f, 0.0f, 1.0f));

    // SECTION("Vectors (with W = 0) should keep pointing in the same direction") {
    const auto projected = XE::project({1.0f, 1.0f, 0.0f, 0.0f}, projViewModel, viewport);
    EXPECT_EQ(projected, XE::Vector4(640.0f, 480.0f, 0.0f, 0.0f));
}

TEST(ProjectionTest, ProjectFunctionPerpective) {
    const XE::Matrix4 proj = XE::Matrix4::perspective(XE::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
    const XE::Matrix4 view = XE::Matrix4::lookAtRH({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    const XE::Matrix4 projViewModel = proj * view;
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    EXPECT_EQ(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport).X, 320.0f);
    EXPECT_EQ(XE::project({0.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport).Y, 240.0f);
}

TEST(ProjectionTest, UnprojectFunction) {
    // SECTION("When considering a default transformation pipeline, with a viewport of (0, 0) - (640, 480)") {
    const XE::Matrix4 projViewModel = XE::Matrix4::identity();
    const XE::Viewport viewport = {{0, 0}, {640, 480}};

    // SECTION("Points placed inside the screen are in the [-1, 1]^2 domain") {
    EXPECT_EQ(XE::unproject({320.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::unproject({0.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(-1.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::unproject({640.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::unproject({320.0f, 480.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::unproject({320.0f, 0.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(0.0f, -1.0f, 0.0f, 1.0f));

    // SECTION("Points placed outside the screen are out of the [-1.0, 1.0]^2") {
    EXPECT_EQ(XE::unproject({-320.0f, 240.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(-2.0f, 0.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::unproject({-320.0f, 720.0f, 0.0f, 1.0f}, projViewModel, viewport), XE::Vector4(-2.0f, 2.0f, 0.0f, 1.0f));
}
