
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

/*
TEST_CASE("Box's attributes change based on values supplied via constructors and mutator methods", "[Box]") {
    XE::Box box {{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    SECTION("getEdge() supplied map a integer index into a specific edge.") {
        REQUIRE(XE::Box::MinEdge >= 0);
        REQUIRE(XE::Box::MaxEdge >= 0);
        REQUIRE(XE::Box::MinEdge < XE::Box::MaxEdge);

        SECTION("getMinEdge() and getMaxEdge() should match the corresponding getEdge() values") {
            REQUIRE(box.getMinEdge() == box.getEdge(XE::Box::MinEdge));
            REQUIRE(box.getMaxEdge() == box.getEdge(XE::Box::MaxEdge));
        }

        SECTION("getEdge() should generate the all the points from the different scalar components of the inner min and max edges") {
            box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

            REQUIRE(box.getEdge(0) == XE::Vector3{-1.0f, -1.0f, -1.0f});
            REQUIRE(box.getEdge(1) == XE::Vector3{1.0f, -1.0f, -1.0f});

            REQUIRE(box.getEdge(2) == XE::Vector3{-1.0f, 1.0f, -1.0f});
            REQUIRE(box.getEdge(3) == XE::Vector3{1.0f, 1.0f, -1.0f});

            REQUIRE(box.getEdge(4) == XE::Vector3{-1.0f, -1.0f, 1.0f});
            REQUIRE(box.getEdge(5) == XE::Vector3{1.0f, -1.0f, 1.0f});

            REQUIRE(box.getEdge(6) == XE::Vector3{-1.0f, 1.0f, 1.0f});
            REQUIRE(box.getEdge(7) == XE::Vector3{1.0f, 1.0f, 1.0f});
        }
    }

    SECTION("getEdge() supplied with an index between (XE::Box::MinEdge - XE::Box::MaxEdge) should return a point different to the Box's minEdge and maxEdge points") {
        int i = XE::Box::MinEdge + 1;

        while (i < XE::Box::MaxEdge) {
            const auto edge = box.getEdge(i);

            REQUIRE(edge != box.getMinEdge());
            REQUIRE(edge != box.getMaxEdge());

            i++;
        }
    }

    SECTION("getSize() should return a vector with the computed size") {
        REQUIRE(box.getSize() == XE::Vector3{2.0f, 6.0f, 6.0f});
    }

    SECTION("getCenter() should return a vector with the computed center of the Box") {
        REQUIRE(box.getCenter() == XE::Vector3{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should not mutate the state when using it with a point inside the Box") {
        box.expand(box.getCenter());

        REQUIRE(box.getSize() == XE::Vector3{2.0f, 6.0f, 6.0f});
        REQUIRE(box.getCenter() == XE::Vector3{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should not mutate the state when using it with a point-like Box inside the Box") {
        box.expand(XE::Box{box.getCenter()});

        REQUIRE(box.getSize() == XE::Vector3{2.0f, 6.0f, 6.0f});
        REQUIRE(box.getCenter() == XE::Vector3{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should mutate the state when using it with a point outside the Box") {
        const auto prevCenter = box.getCenter();
        const auto prevSize = box.getSize();

        box.expand(XE::Vector3{-10.0f, -10.0f, -10.0f});

        REQUIRE(box.getMinEdge() == XE::Vector3{-10.0f, -10.0f, -10.0f});
        REQUIRE(box.getCenter() != prevCenter);
        REQUIRE(box.getSize() != prevSize);
    }
}


TEST_CASE("Boxes can be tested against a Point and another Boxes", "[Box]") {
    XE::Box box {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    SECTION("isInside() should check if a point is inside or outside") {
        REQUIRE(box.isInside({0.0f, 0.0f, 0.0f}));
        REQUIRE(box.isInside({1.0f, 0.0f, 0.0f}));
        REQUIRE(box.isInside({0.0f, 1.0f, 0.0f}));
        REQUIRE(box.isInside({0.0f, 0.0f, 1.0f}));
        REQUIRE(box.isInside({-1.0f, 0.0f, 0.0f}));
        REQUIRE(box.isInside({0.0f, -1.0f, 0.0f}));
        REQUIRE(box.isInside({0.0f, 0.0f, -1.0f}));

        REQUIRE(! box.isInside({10.0f, 0.0f, 0.0f}));
        REQUIRE(! box.isInside({0.0f, 10.0f, 0.0f}));
        REQUIRE(! box.isInside({0.0f, 0.0f, 10.0f}));
    }

    SECTION("isInside() should be True with all the getEdge() points") {
        for (int i=XE::Box::MinEdge; i<=XE::Box::MaxEdge; i++) {
            REQUIRE(box.isInside(box.getEdge(i)));
        }
    }

    SECTION("intersect() should check if two boxes make an intersction or not") {
        SECTION("intersects with itself or an equivalent Box ") {
            REQUIRE(box.intersect(box));
            REQUIRE(box.intersect(XE::Box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}));
            REQUIRE(XE::Box{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}.intersect(box));
        }

        SECTION("intesects between a sub/super box (within)") {
            const XE::Box within{{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};

            REQUIRE(box.intersect(within));
            REQUIRE(within.intersect(box));
        }

        SECTION("intesects when using a box with equal volume, but displaced using half times the size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3 displacements[] = {
                {0.5f * size.X, 0.0f, 0.0f},
                {0.0f, 0.5f * size.Y, 0.0f},
                {0.0f, 0.0f, 0.5f * size.Z},
                {-0.5f * size.X, 0.0f, 0.0f},
                {0.0f, -0.5f * size.Y, 0.0f},
                {0.0f, 0.0f, -0.5f * size.Z},
            };

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Box{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                REQUIRE(box.intersect(displaced));
                REQUIRE(displaced.intersect(box));
            }
        }

        SECTION("should intersect when using a box with equal volume, but displaced using the Subject nearly the size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3 displacements[] = {
                {0.99f * size.X, 0.0f, 0.0f},
                {0.0f, 0.99f * size.Y, 0.0f},
                {0.0f, 0.0f, 0.99f * size.Z},
                {-0.99f * size.X, 0.0f, 0.0f},
                {0.0f, -0.99f * size.Y, 0.0f},
                {0.0f, 0.0f, -0.99f * size.Z},
            };

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Box{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                REQUIRE(box.intersect(displaced));
                REQUIRE(displaced.intersect(box));
            }
        }

        SECTION("should not intersect when using a box with equal volume, but displaced using the Subject size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3 displacements[] = {
                {size.X, 0.0f, 0.0f},
                {0.0f, size.Y, 0.0f},
                {0.0f, 0.0f, size.Z},
                {-size.X, 0.0f, 0.0f},
                {0.0f, -size.Y, 0.0f},
                {0.0f, 0.0f, -size.Z},
            };

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Box{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                REQUIRE(! box.intersect(displaced));
                REQUIRE(! displaced.intersect(box));
            }
        }

        SECTION("not intesects when using a box with equal volume, but displaced using two and half times the size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3 displacements[] = {
                {2.5f * size.X, 0.0f, 0.0f},
                {0.0f, 2.5f * size.Y, 0.0f},
                {0.0f, 0.0f, 2.5f * size.Z},
                {-2.5f * size.X, 0.0f, 0.0f},
                {0.0f, -2.5f * size.Y, 0.0f},
                {0.0f, 0.0f, -2.5f * size.Z},
            };

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Box{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                REQUIRE(! displaced.intersect(box));
                REQUIRE(! box.intersect(displaced));
            }
        }
    }
}


TEST_CASE("Box's can be serialized to an ostream", "[Box]") {
    XE::Box box {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    SECTION("operator<<() should create an unique string representation") {
        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box;
        REQUIRE(ss.str() == ss1.str());
    }

    SECTION("operator<<() should create an unique string representation from different boxes") {
        XE::Box box1 {{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}};

        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box1;
        REQUIRE(ss.str() != ss1.str());
    }
}
*/