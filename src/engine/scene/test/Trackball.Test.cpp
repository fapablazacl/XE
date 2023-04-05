
#include <gtest/gtest.h>
#include <xe/scene/Trackball.h>

TEST(TrackballTest, ConstructorFillsStartCurrentAndEndPoints) {
    EXPECT_EQ(XE::Trackball(XE::Vector2i(640, 480)).getVirtualSphere(), XE::VirtualSphere(XE::Vector2i(640, 480)));
    EXPECT_EQ(XE::Trackball(XE::Vector2i(800, 600)).getVirtualSphere(), XE::VirtualSphere(XE::Vector2i(800, 600)));

    auto trackball = XE::Trackball{{800, 600}};

    trackball.beginDrag({0, 0});
    trackball.drag({50, 50});
    trackball.endDrag({100, 100});

    EXPECT_EQ(trackball.getDragBegin(), XE::Vector2i(0, 0));
    EXPECT_EQ(trackball.getDragCurrent(), XE::Vector2i(50, 50));
    EXPECT_EQ(trackball.getDragEnd(), XE::Vector2i(100, 100));
}

TEST(TrackballTest, ComputeRotationGeneratesAnEmptyRotationWhenStartAndEndPointsAreTheSame) {
    XE::Trackball trackball = {{640, 480}};

    trackball.beginDrag({100, 100});
    trackball.endDrag({100, 100});

    XE::Rotation rotation = trackball.computeRotation();

    EXPECT_EQ(rotation.angle, 0.0);
}

// TODO: These tests should be implemented after the quaternion class is fully unit-tested.
// TEST(TrackballTest, "should generate a rotation around the Y axis with a horizontal mouse movement, inside the virtual unit trackball") {
//    auto trackball = XE::Trackball{{100, 100}};

//    trackball.beginDrag({50, 50});
//    trackball.drag({55, 50});
//    trackball.endDrag({60, 50});

//    auto rotation = trackball.computeRotation();

//    EXPECT_EQ(rotation.axis, XE::Vector3{0.0f, 1.0f, 0.0f});
//    EXPECT_EQ(rotation.angle > 0.0f);
//}

// TEST(TrackballTest, "should generate a rotation around the X axis with a horizontal mouse movement, inside the virtual unit trackball") {
//     auto trackball = XE::Trackball{{100, 100}};

//    trackball.beginDrag({50, 50});
//    trackball.drag({50, 55});
//    trackball.endDrag({50, 60});

//    auto rotation = trackball.computeRotation();

//    EXPECT_EQ(rotation.axis, XE::Vector3{1.0f, 0.0f, 0.0f});
//    EXPECT_EQ(rotation.angle > 0.0f);
//}
