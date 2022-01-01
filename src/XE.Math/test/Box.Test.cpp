
#include <XE/Math/Box.h>
#include <catch2/catch_all.hpp>
#include <sstream>
#include <iostream>


TEST_CASE("Box's can be created using common constructors, with a valid state", "[Box]") {
    SECTION("default constructor should have zero-vector in its edges") {
        XE::Boxf box{};

        REQUIRE(box.getMinEdge() == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(box.isValid());
    }

    SECTION("Boundary(const Vector&) should set both edges with the supplied parameter ") {
        XE::Boxf box{{1.0f, 2.0f, 3.0f}};

        REQUIRE(box.getMinEdge() == XE::Vector3f{1.0f, 2.0f, 3.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{1.0f, 2.0f, 3.0f});
        REQUIRE(box.isValid());
    }

    SECTION("Boundary(const Vector&, const Vector&) should let the box with a Valid state") {
        XE::Boxf box{{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

        REQUIRE(box.getMinEdge() == XE::Vector3f{-1.0f, 2.0f, -3.0f});
        REQUIRE(box.getMaxEdge() == XE::Vector3f{1.0f, 8.0f, 3.0f});
        REQUIRE(box.isValid());
    }
}


TEST_CASE("Box's attributes change based on values supplied via constructors and mutator methods", "[Box]") {
    XE::Boxf box {{1.0f, 2.0f, -3.0f}, {-1.0f, 8.0f, 3.0f}};

    SECTION("getEdge() supplied map a integer index into a specific edge.") {
        REQUIRE(XE::Boxf::MinEdge >= 0);
        REQUIRE(XE::Boxf::MaxEdge >= 0);
        REQUIRE(XE::Boxf::MinEdge < XE::Boxf::MaxEdge);

        SECTION("getMinEdge() and getMaxEdge() should match the corresponding getEdge() values") {
            REQUIRE(box.getMinEdge() == box.getEdge(XE::Boxf::MinEdge));
            REQUIRE(box.getMaxEdge() == box.getEdge(XE::Boxf::MaxEdge));
        }

        SECTION("getEdge() should generate the all the points from the different scalar components of the inner min and max edges") {
            box = {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

            REQUIRE(box.getEdge(0) == XE::Vector3f{-1.0f, -1.0f, -1.0f});
            REQUIRE(box.getEdge(1) == XE::Vector3f{1.0f, -1.0f, -1.0f});
            
            REQUIRE(box.getEdge(2) == XE::Vector3f{-1.0f, 1.0f, -1.0f});
            REQUIRE(box.getEdge(3) == XE::Vector3f{1.0f, 1.0f, -1.0f});

            REQUIRE(box.getEdge(4) == XE::Vector3f{-1.0f, -1.0f, 1.0f});
            REQUIRE(box.getEdge(5) == XE::Vector3f{1.0f, -1.0f, 1.0f});
            
            REQUIRE(box.getEdge(6) == XE::Vector3f{-1.0f, 1.0f, 1.0f});
            REQUIRE(box.getEdge(7) == XE::Vector3f{1.0f, 1.0f, 1.0f});
        }
    }

    SECTION("getEdge() supplied with an index between (XE::Boxf::MinEdge - XE::Boxf::MaxEdge) should return a point different to the Box's minEdge and maxEdge points") {
        int i = XE::Boxf::MinEdge + 1;

        while (i < XE::Boxf::MaxEdge) {
            const auto edge = box.getEdge(i);

            REQUIRE(edge != box.getMinEdge());
            REQUIRE(edge != box.getMaxEdge());

            i++;
        }
    }

    SECTION("getSize() should return a vector with the computed size") {
        REQUIRE(box.getSize() == XE::Vector3f{2.0f, 6.0f, 6.0f});
    }

    SECTION("getCenter() should return a vector with the computed center of the Box") {
        REQUIRE(box.getCenter() == XE::Vector3f{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should not mutate the state when using it with a point inside the Box") {
        box.expand(box.getCenter());

        REQUIRE(box.getSize() == XE::Vector3f{2.0f, 6.0f, 6.0f});
        REQUIRE(box.getCenter() == XE::Vector3f{0.0f, 5.0f, 0.0f});
    }

    SECTION("expand() should not mutate the state when using it with a point-like Box inside the Box") {
        box.expand(XE::Boxf{box.getCenter()});

        REQUIRE(box.getSize() == XE::Vector3f{2.0f, 6.0f, 6.0f});
        REQUIRE(box.getCenter() == XE::Vector3f{0.0f, 5.0f, 0.0f});
    }
    
    SECTION("expand() should mutate the state when using it with a point outside the Box") {
        const auto prevCenter = box.getCenter();
        const auto prevSize = box.getSize();

        box.expand(XE::Vector3f{-10.0f, -10.0f, -10.0f});

        REQUIRE(box.getMinEdge() == XE::Vector3f{-10.0f, -10.0f, -10.0f});
        REQUIRE(box.getCenter() != prevCenter);
        REQUIRE(box.getSize() != prevSize);
    }
}


TEST_CASE("Boxes can be tested against a Point and another Boxes", "[Box]") {
    XE::Boxf box {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

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
        for (int i=XE::Boxf::MinEdge; i<=XE::Boxf::MaxEdge; i++) {
            REQUIRE(box.isInside(box.getEdge(i)));
        }
    }

    SECTION("intersect() should check if two boxes make an intersction or not") {
        SECTION("intersects with itself or an equivalent Box ") {
            REQUIRE(box.intersect(box));
            REQUIRE(box.intersect(XE::Boxf{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}));
            REQUIRE(XE::Boxf{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}.intersect(box));
        }

        SECTION("intesects between a sub/super box (within)") {
            const XE::Boxf within{{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};

            REQUIRE(box.intersect(within));
            REQUIRE(within.intersect(box));
        }
        
        SECTION("intesects when using a box with equal volume, but displaced using half times the size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3f displacements[] = {
                {0.5f * size.X, 0.0f, 0.0f},
                {0.0f, 0.5f * size.Y, 0.0f},
                {0.0f, 0.0f, 0.5f * size.Z},
                {-0.5f * size.X, 0.0f, 0.0f},
                {0.0f, -0.5f * size.Y, 0.0f},
                {0.0f, 0.0f, -0.5f * size.Z},
            };

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Boxf{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                REQUIRE(box.intersect(displaced));
                REQUIRE(displaced.intersect(box));
            }
        }

        SECTION("not intesects when using a box with equal volume, but displaced using one and half times the size in each dimension") {
            const auto size = box.getSize();

            const XE::Vector3f displacements[] = {
                {2.5f * size.X, 0.0f, 0.0f},
                {0.0f, 2.5f * size.Y, 0.0f},
                {0.0f, 0.0f, 2.5f * size.Z},
                {-2.5f * size.X, 0.0f, 0.0f},
                {0.0f, -2.5f * size.Y, 0.0f},
                {0.0f, 0.0f, -2.5f * size.Z},
            };

            std::cout << box << std::endl;

            for (const auto &displacement : displacements) {
                const auto displaced = XE::Boxf{
                    box.getMinEdge() + displacement,
                    box.getMaxEdge() + displacement
                };

                std::cout << displaced << std::endl;
                std::cout << "=================================================" << std::endl;

                REQUIRE(! displaced.intersect(box));
                REQUIRE(! box.intersect(displaced));
            }
        }
    }
}


TEST_CASE("Box's can be serialized to an ostream", "[Box]") {
    XE::Boxf box {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};

    SECTION("operator<<() should create an unique string representation") {
        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box;
        REQUIRE(ss.str() == ss1.str());
    }

    SECTION("operator<<() should create an unique string representation from different boxes") {
        XE::Boxf box1 {{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}};

        std::stringstream ss;
        ss << box;
        REQUIRE(ss.str() != "");

        std::stringstream ss1;
        ss1 << box1;
        REQUIRE(ss.str() != ss1.str());
    }
}
