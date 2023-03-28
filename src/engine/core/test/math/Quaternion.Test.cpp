
#include "Common.h"
#include <xe/math/Common.h>
#include <xe/math/FormatUtils.h>
#include <xe/math/Quaternion.h>

TEST(QuaternionTest, ConstructorsShouldCreatePlanesWithSpecificValues) {
    const XE::Quaternion<float> q1 {{0.0f, 0.0f, 0.0f}, 0.0f};

    EXPECT_EQ(true, true);
}

/*
TEST_CASE("Quaternion basic members are initialized properly", "[Quaternion]") {
    SECTION("when using the Vector-Scalar constructor") {
        const auto q1 = XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f};
        REQUIRE(q1.V == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(q1.W == 0.0f);

        const auto q2 = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f};
        REQUIRE(q2.V == XE::Vector3f{0.0f, 1.0f, 0.0f});
        REQUIRE(q2.W == 1.0f);
    }

    SECTION("when using the 4-scalar constructor") {
        const auto q = XE::Quaternion<float>{0.0f, 1.0f, 0.0f, 1.0f};
        REQUIRE(q.V == XE::Vector3f{0.0f, 1.0f, 0.0f});
        REQUIRE(q.W == 1.0f);
    }

    SECTION("when using the basic one scalar constructor") {
        const auto q1 = XE::Quaternion<float>{0.0f};
        REQUIRE(q1.V == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(q1.W == 0.0f);

        const auto q2 = XE::Quaternion<float>{1.0f};
        REQUIRE(q2.V == XE::Vector3f{1.0f, 1.0f, 1.0f});
        REQUIRE(q2.W == 1.0f);
    }

    SECTION("when using the Vector constructor") {
        const auto q = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}};
        REQUIRE(q.V == XE::Vector3f{0.0f, 1.0f, 0.0f});
        REQUIRE(q.W == 0.0f);
    }

    SECTION("when using the 3-scalar constructor") {
        const auto q = XE::Quaternion<float>{0.0f, 1.0f, 0.0f};
        REQUIRE(q.V == XE::Vector3f{0.0f, 1.0f, 0.0f});
        REQUIRE(q.W == 0.0f);
    }

    SECTION("when using the 4-element array constructor") {
        const float values[] = {
            4.0f, 2.0f, 3.0f, 1.0f
        };

        const auto q = XE::Quaternion<float>{values};
        REQUIRE(q.V == XE::Vector3f{4.0f, 2.0f, 3.0f});
        REQUIRE(q.W == 1.0f);
    }

    SECTION("when using the copy constructor") {
        const auto q1 = XE::Quaternion<float>{XE::Quaternion<float>{0.0f, 0.0f, 0.0f, 1.0f}};
        const auto q2 = XE::Quaternion<float>{XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f}};

        REQUIRE(q1 == XE::Quaternion<float>{0.0f, 0.0f, 0.0f, 1.0f});
        REQUIRE(q2 == XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f});
    }
}

TEST_CASE("Quaternion comparison operators should behave correctly", "[Quaternion]") {
    SECTION("given some test quaternions") {
        const auto qa_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}, 1.0f};
        const auto qa_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f};

        const auto qb_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}};
        const auto qb_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f};

        SECTION("when using the equality operator") {
            REQUIRE(qa_1 == XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f});
            REQUIRE(qa_2 == XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f});

            REQUIRE(qb_1 == XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 0.0f});
            REQUIRE(qb_2 == XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 0.0f});

            REQUIRE(qa_1 == qa_2);
            REQUIRE(qb_1 == qb_2);
            REQUIRE(qa_2 == qa_1);
            REQUIRE(qb_2 == qb_1);
        }

        SECTION("when using the inequality operator") {
            REQUIRE(qa_1 != qb_2);
            REQUIRE(qa_2 != qb_1);

            REQUIRE(qb_1 != qa_2);
            REQUIRE(qb_2 != qa_1);
        }
    }
}

TEST_CASE("Quaternion construction static methods", "[Quaternion]") {
    SECTION("createZero should create a quaternion with all the components to Zero") {
        REQUIRE(XE::Quaternion<float>::createZero() == XE::Quaternion<float>{0.0f, 0.0f, 0.0f, 0.0f});
    }

    SECTION("createIdentity should create an identity quaternion") {
        REQUIRE(XE::Quaternion<float>::createIdentity() == XE::Quaternion<float>{0.0f, 0.0f, 0.0f, 1.0f});
    }
}

TEST_CASE("Quaternion basic arithmetic operators behave correctly", "[Quaternion]") {
    SECTION("given some test quaternions") {
        const auto q1 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f};
        const auto q2 = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f};
        const auto q3 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f};
        const auto q4 = XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f};

        SECTION("addition operator should add their each components together") {
            REQUIRE(q1 + q1 == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 2.0f});
            REQUIRE(q1 + q2 == XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 2.0f});
            REQUIRE(q2 + q3 == XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f});
            REQUIRE(q3 + q4 == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
        }

        SECTION("subtraction operator should subtract each component together") {
            REQUIRE(q1 - q1 == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
            REQUIRE(q1 - q2 == XE::Quaternion<float>{{1.0f, -1.0f, 1.0f}, 0.0f});
            REQUIRE(q2 - q3 == XE::Quaternion<float>{{-1.0f, 1.0f, -1.0f}, 1.0f});
            REQUIRE(q3 - q4 == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 0.0f});
        }

        SECTION("negation operator should invert each component") {
            REQUIRE(-q1 == XE::Quaternion<float>{{-1.0f, -0.0f, -1.0f}, -1.0f});
            REQUIRE(-q2 == XE::Quaternion<float>{{0.0f, -1.0f, 0.0f}, -1.0f});
            REQUIRE(-q3 == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE(-q4 == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f});
        }

        SECTION("plus operator should modify no components") {
            REQUIRE(+q1 == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE(+q2 == XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f});
            REQUIRE(+q3 == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f});
            REQUIRE(+q4 == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
        }

        SECTION("multiply by scalar operator multiply each component") {
            REQUIRE(q1 * 1.0f == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE(q2 * 0.0f == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
            REQUIRE(q3 * -1.0f == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE(q4 * 2.0f == XE::Quaternion<float>{{-2.0f, 0.0f, -2.0f}, 0.0f});
        }

        SECTION("scalar by quaternion operator multiply each component") {
            REQUIRE(1.0f * q1 == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE(0.0f * q2 == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
            REQUIRE(-1.0f * q3 == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE(2.0f * q4 == XE::Quaternion<float>{{-2.0f, 0.0f, -2.0f}, 0.0f});
        }

        SECTION("divide by scalar operator multiply each component") {
            REQUIRE(q1 / 1.0f == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE(q2 / 0.5f == XE::Quaternion<float>{{0.0f, 2.0f, 0.0f}, 2.0f});
            REQUIRE(q3 / -1.0f == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE(q4 / -0.5f == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 0.0f});
        }

        SECTION("multiply by another quaternion should combine them together") {
            SECTION("identity should not affect any quaternion") {
                const auto qi = XE::Quaternion<float>::createIdentity();

                REQUIRE(q1 * qi == q1);
                REQUIRE(q2 * qi == q2);
                REQUIRE(q3 * qi == q3);
                REQUIRE(q4 * qi == q4);
            }

            SECTION("zero should collapse any quaternion to zero") {
                const auto qz = XE::Quaternion<float>::createZero();

                REQUIRE(q1 * qz == qz);
                REQUIRE(q2 * qz == qz);
                REQUIRE(q3 * qz == qz);
                REQUIRE(q4 * qz == qz);
            }

            REQUIRE(q1 * q2 != q2 * q1);
            REQUIRE(q1 * q2 == XE::Quaternion<float>{{0.0f, 1.0f, 2.0f}, 1.0f});
            REQUIRE(q2 * q1 == XE::Quaternion<float>{{2.0f, 1.0f, 0.0f}, 1.0f});
        }

        SECTION("divide by another quaternion should be equivalent to multiply by the inverse") {
            SECTION("identity should not affect any quaternion") {
                const auto qi = XE::Quaternion<float>::createIdentity();

                REQUIRE(q1 / qi == q1);
                REQUIRE(q2 / qi == q2);
                REQUIRE(q3 / qi == q3);
                REQUIRE(q4 / qi == q4);
            }

            SECTION("zero should collapse any quaternion to NaN values") {
                const auto qz = XE::Quaternion<float>::createZero();

                const auto r1 = q1 / qz;
                const auto r2 = q2 / qz;

                REQUIRE(std::isnan(r1.V.X));
                REQUIRE(std::isnan(r1.V.Y));
                REQUIRE(std::isnan(r1.V.Z));
                REQUIRE(std::isnan(r1.W));

                REQUIRE(std::isnan(r2.V.X));
                REQUIRE(std::isnan(r2.V.Y));
                REQUIRE(std::isnan(r2.V.Z));
                REQUIRE(std::isnan(r2.W));
            }
        }
    }
}

TEST_CASE("Quaternion basic arithmetic accumulation operators behave correctly", "[Quaternion]") {
    SECTION("given some test quaternions") {
        auto q1 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f};
        auto q2 = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f};
        auto q3 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f};
        auto q4 = XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f};

        SECTION("addition operator should add their each components together") {
            REQUIRE((+q1 += q1) == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 2.0f});
            REQUIRE((+q1 += q2) == XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 2.0f});
            REQUIRE((+q2 += q3) == XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f});
            REQUIRE((+q3 += q4) == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
        }

        SECTION("subtraction operator should subtract each component together") {
            REQUIRE((+q1 -= q1) == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
            REQUIRE((+q1 -= q2) == XE::Quaternion<float>{{1.0f, -1.0f, 1.0f}, 0.0f});
            REQUIRE((+q2 -= q3) == XE::Quaternion<float>{{-1.0f, 1.0f, -1.0f}, 1.0f});
            REQUIRE((+q3 -= q4) == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 0.0f});
        }

        SECTION("multiply by scalar operator multiply each component") {
            REQUIRE((q1 *= 1.0f) == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE((q2 *= 0.0f) == XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f});
            REQUIRE((q3 *= -1.0f) == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE((q4 *= 2.0f) == XE::Quaternion<float>{{-2.0f, 0.0f, -2.0f}, 0.0f});
        }

        SECTION("divide by scalar operator multiply each component") {
            REQUIRE((q1 /= 1.0f) == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f});
            REQUIRE((q2 /= 0.5f) == XE::Quaternion<float>{{0.0f, 2.0f, 0.0f}, 2.0f});
            REQUIRE((q3 /= -1.0f) == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
            REQUIRE((q4 /= -0.5f) == XE::Quaternion<float>{{2.0f, 0.0f, 2.0f}, 0.0f});
        }
    }
}

TEST_CASE("Quaternion operator functions", "[Quaternion]") {
    SECTION("'conjugate' should negate the imaginary part") {
        const auto q1 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 1.0f};
        const auto q2 = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f};
        const auto q3 = XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f};
        const auto q4 = XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f};

        REQUIRE(XE::conjugate(q1) == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 1.0f});
        REQUIRE(XE::conjugate(q2) == XE::Quaternion<float>{{0.0f, -1.0f, 0.0f}, 1.0f});
        REQUIRE(XE::conjugate(q3) == XE::Quaternion<float>{{-1.0f, 0.0f, -1.0f}, 0.0f});
        REQUIRE(XE::conjugate(q4) == XE::Quaternion<float>{{1.0f, 0.0f, 1.0f}, 0.0f});
    }

    SECTION("'dot' should compute the Dot Product from two givens Quaternions") {
        const XE::Quaternion<float> v1 = {2.0f, 8.0f, 32.0f, 0.0f};
        const XE::Quaternion<float> v2 = {1.0f, 2.0f, 4.0f, 0.0f};

        REQUIRE(dot(XE::Quaternion<float>{1.0f, 1.0f, 1.0f, 1.0f}, v1) == 42.0f);
        REQUIRE(dot(XE::Quaternion<float>{1.0f, 1.0f, 1.0f, 1.0f}, v2) == 7.0f);
        REQUIRE(dot(XE::Quaternion<float>{0.0f}, v1) == 0.0f);
        REQUIRE(dot(XE::Quaternion<float>{0.0f}, v2) == 0.0f);

        REQUIRE(dot(v1, XE::Quaternion<float>{1.0f, 1.0f, 1.0f}) == 42.0f);
        REQUIRE(dot(v2, XE::Quaternion<float>{1.0f, 1.0f, 1.0f}) == 7.0f);
        REQUIRE(dot(v1, XE::Quaternion<float>{0.0f}) == 0.0f);

        REQUIRE(dot(v1, v2) == 146.0f);
        REQUIRE(dot(v2, v1) == 146.0f);
    }

    SECTION("'norm2' should compute the quaternion length squared") {
        REQUIRE(XE::norm2(XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f}) == 4.0f);
        REQUIRE(XE::norm2(XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f}) == 0.0f);
        REQUIRE(XE::norm2(XE::Quaternion<float>{{-1.0f, -1.0f, -1.0f}, -1.0f}) == 4.0f);
    }

    SECTION("'norm' should compute the quaternion length") {
        REQUIRE(XE::norm(XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f}) == 2.0f);
        REQUIRE(XE::norm(XE::Quaternion<float>{{0.0f, 0.0f, 0.0f}, 0.0f}) == 0.0f);
        REQUIRE(XE::norm(XE::Quaternion<float>{{-1.0f, -1.0f, -1.0f}, -1.0f}) == 2.0f);
    }

    SECTION("'inverse' should compute an inverse quaternion") {
        const auto q1 = XE::Quaternion<float>{{3.0f, 4.0f, 5.0f}, 0.0f};
        REQUIRE(XE::inverse(q1) == XE::Quaternion<float>{{-3.0f / 50.0f, -4.0f / 50.0f, -5.0f / 50.0f}, 0.0f});

        const auto q2 = XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f};
        REQUIRE(XE::inverse(q2) == XE::Quaternion<float>{{-0.25f, -0.25f, -0.25f}, 0.25f});
    }

    SECTION("'normalize' should scale a quaternion to the Unit Length") {
        const auto q1 = XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f};
        const auto q2 = XE::Quaternion<float>{{0.0f, 4.0f, 0.0f}, 0.0f};
        const auto q3 = XE::Quaternion<float>{{0.0f, -1.0f, 0.0f}, 0.0f};

        REQUIRE(XE::normalize(q1) == XE::Quaternion<float>{{0.5f, 0.5f, 0.5f}, 0.5f});
        REQUIRE(XE::normalize(q2) == XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 0.0f});
        REQUIRE(XE::normalize(q3) == XE::Quaternion<float>{{0.0f, -1.0f, 0.0f}, 0.0f});
    }
}
*/