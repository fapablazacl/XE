
#include "xe/math/Box.h"
#include "GoogleTestCommon.h"
#include <iostream>
#include <sstream>

TEST(BoxTest, DefaultConstructorZeroesEdges) {
    XE::Box box;

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, ConstructorFromOneVectorInitializeEdges) {
    XE::Box box{{1.0f, 2.0f, 3.0f}};

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, ConstructorFromTwoVectorsInitalizeEdges) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(-1.0f, 2.0f, -3.0f));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(1.0f, 8.0f, 3.0f));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, MinAndMaxEdge) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    EXPECT_GE(XE::Box::MinEdge, 0.0f);
    EXPECT_GE(XE::Box::MaxEdge, 0.0f);
    EXPECT_LT(XE::Box::MinEdge, XE::Box::MaxEdge);
}

TEST(BoxTest, getMinEdgeAndGetMaxEdgeMatchesGetEdgeValues) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    EXPECT_EQ(box.getMinEdge(), box.getEdge(XE::Box::MinEdge));
    EXPECT_EQ(box.getMaxEdge(), box.getEdge(XE::Box::MaxEdge));
}

TEST(BoxTest, getEdgeShouldGetASpecificPointFromTheBox) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    EXPECT_EQ(box.getEdge(0), XE::Vector3(-1.0f, -1.0f, -1.0f));
    EXPECT_EQ(box.getEdge(1), XE::Vector3(1.0f, -1.0f, -1.0f));

    EXPECT_EQ(box.getEdge(2), XE::Vector3(-1.0f, 1.0f, -1.0f));
    EXPECT_EQ(box.getEdge(3), XE::Vector3(1.0f, 1.0f, -1.0f));

    EXPECT_EQ(box.getEdge(4), XE::Vector3(-1.0f, -1.0f, 1.0f));
    EXPECT_EQ(box.getEdge(5), XE::Vector3(1.0f, -1.0f, 1.0f));

    EXPECT_EQ(box.getEdge(6), XE::Vector3(-1.0f, 1.0f, 1.0f));
    EXPECT_EQ(box.getEdge(7), XE::Vector3(1.0f, 1.0f, 1.0f));
}

TEST(BoxTest, getSizeReturnsAVectorWithTheComputedSize) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};
    EXPECT_EQ(box.getSize(), XE::Vector3(2.0f, 6.0f, 6.0f));
}

TEST(BoxTest, getCenterReturnsAVectorWithTheAverageBetweenMinAndMaxEdges) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0f, 5.0f, 0.0f));
}

TEST(BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointInsideOfIt) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box.expand(box.getCenter());

    EXPECT_EQ(box.getSize(), XE::Vector3(2.0f, 6.0f, 6.0f));
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0f, 5.0f, 0.0f));
}

TEST(BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointLikeBoxInsideOfIt) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box.expand(XE::Box{box.getCenter()});

    EXPECT_EQ(box.getSize(), XE::Vector3(2.0f, 6.0f, 6.0f));
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0f, 5.0f, 0.0f));
}

TEST(BoxTest, expandIncreasesTheBoxWhenUsingAPointInsideOf) {
    XE::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    const auto prevCenter = box.getCenter();
    const auto prevSize = box.getSize();

    box.expand(XE::Vector3(-10.0f, -10.0f, -10.0f));

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(-10.0f, -10.0f, -10.0f));
    EXPECT_NE(box.getCenter(), prevCenter);
    EXPECT_NE(box.getSize(), prevSize);
}

TEST(BoxTest, isInsideChecksIfAPointBetweenTheMinAndMaxEdges) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    EXPECT_TRUE(box.isInside({0.0f, 0.0f, 0.0f}));
    EXPECT_TRUE(box.isInside({1.0f, 0.0f, 0.0f}));
    EXPECT_TRUE(box.isInside({0.0f, 1.0f, 0.0f}));
    EXPECT_TRUE(box.isInside({0.0f, 0.0f, 1.0f}));
    EXPECT_TRUE(box.isInside({-1.0f, 0.0f, 0.0f}));
    EXPECT_TRUE(box.isInside({0.0f, -1.0f, 0.0f}));
    EXPECT_TRUE(box.isInside({0.0f, 0.0f, -1.0f}));

    EXPECT_FALSE(box.isInside({10.0f, 0.0f, 0.0f}));
    EXPECT_FALSE(box.isInside({0.0f, 10.0f, 0.0f}));
    EXPECT_FALSE(box.isInside({0.0f, 0.0f, 10.0f}));

    for (int i = XE::Box::MinEdge; i <= XE::Box::MaxEdge; i++) {
        EXPECT_TRUE(box.isInside(box.getEdge(i)));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxes) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    XE::Box box2 = XE::Box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    EXPECT_TRUE(box.intersect(box));
    EXPECT_TRUE(box.intersect(box2));
    EXPECT_TRUE(box2.intersect(box));

    const XE::Box within{{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};
    EXPECT_TRUE(box.intersect(within));
    EXPECT_TRUE(within.intersect(box));
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingWithAHalfSizedBox) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {0.5f * size.X, 0.0f, 0.0f},  {0.0f, 0.5f * size.Y, 0.0f},  {0.0f, 0.0f, 0.5f * size.Z},
        {-0.5f * size.X, 0.0f, 0.0f}, {0.0f, -0.5f * size.Y, 0.0f}, {0.0f, 0.0f, -0.5f * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_TRUE(box.intersect(displaced));
        EXPECT_TRUE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithANearlyPosition) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {0.99f * size.X, 0.0f, 0.0f},  {0.0f, 0.99f * size.Y, 0.0f},  {0.0f, 0.0f, 0.99f * size.Z},
        {-0.99f * size.X, 0.0f, 0.0f}, {0.0f, -0.99f * size.Y, 0.0f}, {0.0f, 0.0f, -0.99f * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_TRUE(box.intersect(displaced));
        EXPECT_TRUE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithExactSizeApart) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {size.X, 0.0f, 0.0f}, {0.0f, size.Y, 0.0f}, {0.0f, 0.0f, size.Z}, {-size.X, 0.0f, 0.0f}, {0.0f, -size.Y, 0.0f}, {0.0f, 0.0f, -size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_FALSE(box.intersect(displaced));
        EXPECT_FALSE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesDisplacesTwoAndAHalfOfTheFirstBoxsize) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {2.5f * size.X, 0.0f, 0.0f},  {0.0f, 2.5f * size.Y, 0.0f},  {0.0f, 0.0f, 2.5f * size.Z},
        {-2.5f * size.X, 0.0f, 0.0f}, {0.0f, -2.5f * size.Y, 0.0f}, {0.0f, 0.0f, -2.5f * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_FALSE(displaced.intersect(box));
        EXPECT_FALSE(box.intersect(displaced));
    }
}

TEST(BoxTest, operatorLessLessShouldCreateAnUniqueStringRepresentationFromDifferentBoxes) {
    XE::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    XE::Box box1{{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}};

    std::stringstream ss;
    ss << box;
    EXPECT_NE(ss.str(), "");

    std::stringstream ss1;
    ss1 << box1;
    EXPECT_NE(ss.str(), ss1.str());
}
