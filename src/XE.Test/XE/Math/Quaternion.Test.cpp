
#include <XE/Math/Quaternion.hpp>
#include <XE/Math/FormatUtils.hpp>
#include <catch.hpp>

static const int precision = std::numeric_limits<float>::max_digits10;

namespace Catch {
    template<>
    struct StringMaker<XE::Quaternion<float>> {
        static std::string convert(XE::Quaternion<float> const& value) {
            return XE::toString(value, precision);
        }
    };
}

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

            REQUIRE(qb_1 == qa_2);
            REQUIRE(qb_2 == qa_1);
        }
    }
}
