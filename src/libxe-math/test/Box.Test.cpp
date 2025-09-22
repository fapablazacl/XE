
#include "xe/math/Box.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(BoxTest, DefaultConstructorZeroesEdges) {
    XE::Box const box;

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(0.0F, 0.0F, 0.0F));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(0.0F, 0.0F, 0.0F));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, ConstructorFromOneVectorInitializeEdges) {
    XE::Box const box{{1.0F, 2.0F, 3.0F}};

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(1.0F, 2.0F, 3.0F));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(1.0F, 2.0F, 3.0F));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, ConstructorFromTwoVectorsInitalizeEdges) {
    XE::Box const box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(-1.0F, 2.0F, -3.0F));
    EXPECT_EQ(box.getMaxEdge(), XE::Vector3(1.0F, 8.0F, 3.0F));
    EXPECT_EQ(box.isValid(), true);
}

TEST(BoxTest, MinAndMaxEdge) {
    XE::Box const box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    EXPECT_GE(XE::Box::MinEdge, 0.0F);
    EXPECT_GE(XE::Box::MaxEdge, 0.0F);
    EXPECT_LT(XE::Box::MinEdge, XE::Box::MaxEdge);
}

TEST(BoxTest, getMinEdgeAndGetMaxEdgeMatchesGetEdgeValues) {
    XE::Box const box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    EXPECT_EQ(box.getMinEdge(), box.getEdge(XE::Box::MinEdge));
    EXPECT_EQ(box.getMaxEdge(), box.getEdge(XE::Box::MaxEdge));
}

TEST(BoxTest, getEdgeShouldGetASpecificPointFromTheBox) {
    XE::Box box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    box = {{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    EXPECT_EQ(box.getEdge(0), XE::Vector3(-1.0F, -1.0F, -1.0F));
    EXPECT_EQ(box.getEdge(1), XE::Vector3(1.0F, -1.0F, -1.0F));

    EXPECT_EQ(box.getEdge(2), XE::Vector3(-1.0F, 1.0F, -1.0F));
    EXPECT_EQ(box.getEdge(3), XE::Vector3(1.0F, 1.0F, -1.0F));

    EXPECT_EQ(box.getEdge(4), XE::Vector3(-1.0F, -1.0F, 1.0F));
    EXPECT_EQ(box.getEdge(5), XE::Vector3(1.0F, -1.0F, 1.0F));

    EXPECT_EQ(box.getEdge(6), XE::Vector3(-1.0F, 1.0F, 1.0F));
    EXPECT_EQ(box.getEdge(7), XE::Vector3(1.0F, 1.0F, 1.0F));
}

TEST(BoxTest, getSizeReturnsAVectorWithTheComputedSize) {
    XE::Box const box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};
    EXPECT_EQ(box.getSize(), XE::Vector3(2.0F, 6.0F, 6.0F));
}

TEST(BoxTest, getCenterReturnsAVectorWithTheAverageBetweenMinAndMaxEdges) {
    XE::Box const box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0F, 5.0F, 0.0F));
}

TEST(BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointInsideOfIt) {
    XE::Box box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    box.expand(box.getCenter());

    EXPECT_EQ(box.getSize(), XE::Vector3(2.0F, 6.0F, 6.0F));
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0F, 5.0F, 0.0F));
}

TEST(BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointLikeBoxInsideOfIt) {
    XE::Box box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    box.expand(XE::Box{box.getCenter()});

    EXPECT_EQ(box.getSize(), XE::Vector3(2.0F, 6.0F, 6.0F));
    EXPECT_EQ(box.getCenter(), XE::Vector3(0.0F, 5.0F, 0.0F));
}

TEST(BoxTest, expandIncreasesTheBoxWhenUsingAPointInsideOf) {
    XE::Box box{{1.0F, 2.0F, -3.0F}, {-1.0F, 8.0F, 3.0F}};

    const auto prevCenter = box.getCenter();
    const auto prevSize = box.getSize();

    box.expand(XE::Vector3(-10.0F, -10.0F, -10.0F));

    EXPECT_EQ(box.getMinEdge(), XE::Vector3(-10.0F, -10.0F, -10.0F));
    EXPECT_NE(box.getCenter(), prevCenter);
    EXPECT_NE(box.getSize(), prevSize);
}

TEST(BoxTest, isInsideChecksIfAPointBetweenTheMinAndMaxEdges) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    EXPECT_TRUE(box.isInside({0.0F, 0.0F, 0.0F}));
    EXPECT_TRUE(box.isInside({1.0F, 0.0F, 0.0F}));
    EXPECT_TRUE(box.isInside({0.0F, 1.0F, 0.0F}));
    EXPECT_TRUE(box.isInside({0.0F, 0.0F, 1.0F}));
    EXPECT_TRUE(box.isInside({-1.0F, 0.0F, 0.0F}));
    EXPECT_TRUE(box.isInside({0.0F, -1.0F, 0.0F}));
    EXPECT_TRUE(box.isInside({0.0F, 0.0F, -1.0F}));

    EXPECT_FALSE(box.isInside({10.0F, 0.0F, 0.0F}));
    EXPECT_FALSE(box.isInside({0.0F, 10.0F, 0.0F}));
    EXPECT_FALSE(box.isInside({0.0F, 0.0F, 10.0F}));

    for (int i = XE::Box::MinEdge; i <= XE::Box::MaxEdge; i++) {
        EXPECT_TRUE(box.isInside(box.getEdge(i)));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxes) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};
    XE::Box const box2 = XE::Box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    EXPECT_TRUE(box.intersect(box));
    EXPECT_TRUE(box.intersect(box2));
    EXPECT_TRUE(box2.intersect(box));

    const XE::Box within{{-0.5F, -0.5F, -0.5F}, {0.5F, 0.5F, 0.5F}};
    EXPECT_TRUE(box.intersect(within));
    EXPECT_TRUE(within.intersect(box));
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingWithAHalfSizedBox) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {0.5F * size.X, 0.0F, 0.0F},
        {0.0F, 0.5F * size.Y, 0.0F},
        {0.0F, 0.0F, 0.5F * size.Z},
        {-0.5F * size.X, 0.0F, 0.0F},
        {0.0F, -0.5F * size.Y, 0.0F},
        {0.0F, 0.0F, -0.5F * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_TRUE(box.intersect(displaced));
        EXPECT_TRUE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithANearlyPosition) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {0.99F * size.X, 0.0F, 0.0F},
        {0.0F, 0.99F * size.Y, 0.0F},
        {0.0F, 0.0F, 0.99F * size.Z},
        {-0.99F * size.X, 0.0F, 0.0F},
        {0.0F, -0.99F * size.Y, 0.0F},
        {0.0F, 0.0F, -0.99F * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_TRUE(box.intersect(displaced));
        EXPECT_TRUE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithExactSizeApart) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};
    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {size.X, 0.0F, 0.0F},
        {0.0F, size.Y, 0.0F},
        {0.0F, 0.0F, size.Z},
        {-size.X, 0.0F, 0.0F},
        {0.0F, -size.Y, 0.0F},
        {0.0F, 0.0F, -size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_FALSE(box.intersect(displaced));
        EXPECT_FALSE(displaced.intersect(box));
    }
}

TEST(BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesDisplacesTwoAndAHalfOfTheFirstBoxsize) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};
    const auto size = box.getSize();

    const XE::Vector3 displacements[] = {
        {2.5F * size.X, 0.0F, 0.0F},
        {0.0F, 2.5F * size.Y, 0.0F},
        {0.0F, 0.0F, 2.5F * size.Z},
        {-2.5F * size.X, 0.0F, 0.0F},
        {0.0F, -2.5F * size.Y, 0.0F},
        {0.0F, 0.0F, -2.5F * size.Z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = XE::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        EXPECT_FALSE(displaced.intersect(box));
        EXPECT_FALSE(box.intersect(displaced));
    }
}

TEST(BoxTest, operatorLessLessShouldCreateAnUniqueStringRepresentationFromDifferentBoxes) {
    XE::Box const box{{-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F}};

    XE::Box const box1{{0.0F, 0.0F, 0.0F}, {10.0F, 10.0F, 10.0F}};

    std::stringstream ss;
    ss << box;
    EXPECT_NE(ss.str(), "");

    std::stringstream ss1;
    ss1 << box1;
    EXPECT_NE(ss.str(), ss1.str());
}
