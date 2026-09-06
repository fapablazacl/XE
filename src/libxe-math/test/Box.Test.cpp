
#include "xe/math/Box.h"
#include <catch2/catch_all.hpp>
#include <iostream>
#include <sstream>

TEST_CASE("BoxTest, DefaultConstructorZeroesEdges") {
    xe::Box box;

    REQUIRE(box.getMinEdge() == xe::Vector3(0.0f, 0.0f, 0.0f));
    REQUIRE(box.getMaxEdge() == xe::Vector3(0.0f, 0.0f, 0.0f));
    REQUIRE(box.isValid());
}

TEST_CASE("BoxTest, ConstructorFromOneVectorInitializeEdges") {
    xe::Box box{{1.0f, 2.0f, 3.0f}};

    REQUIRE(box.getMinEdge() == xe::Vector3(1.0f, 2.0f, 3.0f));
    REQUIRE(box.getMaxEdge() == xe::Vector3(1.0f, 2.0f, 3.0f));
    REQUIRE(box.isValid());
}

TEST_CASE("BoxTest, ConstructorFromTwoVectorsInitalizeEdges") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    REQUIRE(box.getMinEdge() == xe::Vector3(-1.0f, 2.0f, -3.0f));
    REQUIRE(box.getMaxEdge() == xe::Vector3(1.0f, 8.0f, 3.0f));
    REQUIRE(box.isValid());
}

TEST_CASE("BoxTest, getEdgeShouldGetASpecificPointFromTheBox") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    REQUIRE(box.getEdge(0) == xe::Vector3(-1.0f, -1.0f, -1.0f));
    REQUIRE(box.getEdge(1) == xe::Vector3(1.0f, -1.0f, -1.0f));

    REQUIRE(box.getEdge(2) == xe::Vector3(-1.0f, 1.0f, -1.0f));
    REQUIRE(box.getEdge(3) == xe::Vector3(1.0f, 1.0f, -1.0f));

    REQUIRE(box.getEdge(4) == xe::Vector3(-1.0f, -1.0f, 1.0f));
    REQUIRE(box.getEdge(5) == xe::Vector3(1.0f, -1.0f, 1.0f));

    REQUIRE(box.getEdge(6) == xe::Vector3(-1.0f, 1.0f, 1.0f));
    REQUIRE(box.getEdge(7) == xe::Vector3(1.0f, 1.0f, 1.0f));
}

TEST_CASE("BoxTest, getSizeReturnsAVectorWithTheComputedSize") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};
    REQUIRE(box.getSize() == xe::Vector3(2.0f, 6.0f, 6.0f));
}

TEST_CASE("BoxTest, getCenterReturnsAVectorWithTheAverageBetweenMinAndMaxEdges") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};
    REQUIRE(box.getCenter() == xe::Vector3(0.0f, 5.0f, 0.0f));
}

TEST_CASE("BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointInsideOfIt") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box.expand(box.getCenter());

    REQUIRE(box.getSize() == xe::Vector3(2.0f, 6.0f, 6.0f));
    REQUIRE(box.getCenter() == xe::Vector3(0.0f, 5.0f, 0.0f));
}

TEST_CASE("BoxTest, expandDoesNotChangesTheBoxWhenUsingAPointLikeBoxInsideOfIt") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    box.expand(xe::Box{box.getCenter()});

    REQUIRE(box.getSize() == xe::Vector3(2.0f, 6.0f, 6.0f));
    REQUIRE(box.getCenter() == xe::Vector3(0.0f, 5.0f, 0.0f));
}

TEST_CASE("BoxTest, expandIncreasesTheBoxWhenUsingAPointInsideOf") {
    xe::Box box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    const auto prevCenter = box.getCenter();
    const auto prevSize = box.getSize();

    box.expand(xe::Vector3(-10.0f, -10.0f, -10.0f));

    REQUIRE(box.getMinEdge() == xe::Vector3(-10.0f, -10.0f, -10.0f));
    EXPECT_NE(box.getCenter(), prevCenter);
    EXPECT_NE(box.getSize(), prevSize);
}

TEST_CASE("BoxTest, isInsideChecksIfAPointBetweenTheMinAndMaxEdges") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    REQUIRE(box.isInside({0.0f, 0.0f, 0.0f}));
    REQUIRE(box.isInside({1.0f, 0.0f, 0.0f}));
    REQUIRE(box.isInside({0.0f, 1.0f, 0.0f}));
    REQUIRE(box.isInside({0.0f, 0.0f, 1.0f}));
    REQUIRE(box.isInside({-1.0f, 0.0f, 0.0f}));
    REQUIRE(box.isInside({0.0f, -1.0f, 0.0f}));
    REQUIRE(box.isInside({0.0f, 0.0f, -1.0f}));

    REQUIRE_FALSE(box.isInside({10.0f, 0.0f, 0.0f}));
    REQUIRE_FALSE(box.isInside({0.0f, 10.0f, 0.0f}));
    REQUIRE_FALSE(box.isInside({0.0f, 0.0f, 10.0f}));

    for (int i = xe::Box::MinEdge; i <= xe::Box::MaxEdge; i++) {
        REQUIRE(box.isInside(box.getEdge(i)));
    }
}

TEST_CASE("BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxes") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    xe::Box box2 = xe::Box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    REQUIRE(box.intersect(box));
    REQUIRE(box.intersect(box2));
    REQUIRE(box2.intersect(box));

    const xe::Box within{{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};
    REQUIRE(box.intersect(within));
    REQUIRE(within.intersect(box));
}

TEST_CASE("BoxTest, intersectsChecksIfThereIsAnOverlappingWithAHalfSizedBox") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    const auto size = box.getSize();

    const xe::Vector3 displacements[] = {
        {0.5f * size.x, 0.0f, 0.0f},
        {0.0f, 0.5f * size.y, 0.0f},
        {0.0f, 0.0f, 0.5f * size.z},
        {-0.5f * size.x, 0.0f, 0.0f},
        {0.0f, -0.5f * size.y, 0.0f},
        {0.0f, 0.0f, -0.5f * size.z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = xe::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        REQUIRE(box.intersect(displaced));
        REQUIRE(displaced.intersect(box));
    }
}

TEST_CASE("BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithANearlyPosition") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    const auto size = box.getSize();

    const xe::Vector3 displacements[] = {
        {0.99f * size.x, 0.0f, 0.0f},
        {0.0f, 0.99f * size.y, 0.0f},
        {0.0f, 0.0f, 0.99f * size.z},
        {-0.99f * size.x, 0.0f, 0.0f},
        {0.0f, -0.99f * size.y, 0.0f},
        {0.0f, 0.0f, -0.99f * size.z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = xe::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        REQUIRE(box.intersect(displaced));
        REQUIRE(displaced.intersect(box));
    }
}

TEST_CASE("BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesWithExactSizeApart") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const auto size = box.getSize();

    const xe::Vector3 displacements[] = {
        {size.x, 0.0f, 0.0f},
        {0.0f, size.y, 0.0f},
        {0.0f, 0.0f, size.z},
        {-size.x, 0.0f, 0.0f},
        {0.0f, -size.y, 0.0f},
        {0.0f, 0.0f, -size.z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = xe::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        REQUIRE_FALSE(box.intersect(displaced));
        REQUIRE_FALSE(displaced.intersect(box));
    }
}

TEST_CASE("BoxTest, intersectsChecksIfThereIsAnOverlappingBetweenTwoBoxesDisplacesTwoAndAHalfOfTheFirstBoxsize") {
    xe::Box box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    const auto size = box.getSize();

    const xe::Vector3 displacements[] = {
        {2.5f * size.x, 0.0f, 0.0f},
        {0.0f, 2.5f * size.y, 0.0f},
        {0.0f, 0.0f, 2.5f * size.z},
        {-2.5f * size.x, 0.0f, 0.0f},
        {0.0f, -2.5f * size.y, 0.0f},
        {0.0f, 0.0f, -2.5f * size.z},
    };

    for (const auto &displacement : displacements) {
        const auto displaced = xe::Box{box.getMinEdge() + displacement, box.getMaxEdge() + displacement};

        REQUIRE_FALSE(displaced.intersect(box));
        REQUIRE_FALSE(box.intersect(displaced));
    }
}
