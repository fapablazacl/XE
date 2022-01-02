
#include <XE/Math/Range.h>
#include <catch2/catch_all.hpp>

TEST_CASE("Range should have a valid state when perform correctly after each kind of operation", "[Range]") {
    XE::Range<float> subject;

    SECTION("default constructor should let the range with values 0 and 1") {
        REQUIRE(subject.max >= subject.min);
        REQUIRE(subject.min == 0.0f);
        REQUIRE(subject.max == 1.0f);
    }

    SECTION("two param constructor should let the range with those values correctly setted") {
        subject = XE::Range<float>{1.0f, -1.0f};

        REQUIRE(subject.max >= subject.min);
        REQUIRE(subject.min == -1.0f);
        REQUIRE(subject.max == 1.0f);
    }

    SECTION("expand(const T) should not mutate the range if the supplied value is inside the range") {
        const float values[] = {
            0.0f, 0.25f, 0.5f, 0.75f, 1.0f
        };

        for (const float value : values) {
            subject.expand(value);
            REQUIRE(subject.max >= subject.min);
            REQUIRE(subject.min == 0.0f);
            REQUIRE(subject.max == 1.0f);
        }
    }

    SECTION("expand(const T) should mutate the range if the supplied value is outside the range") {
        subject.expand(-1.0f);
        REQUIRE(subject.max >= subject.min);
        REQUIRE(subject.min == -1.0f);
        REQUIRE(subject.max == 1.0f);

        subject.expand(2.0f);
        REQUIRE(subject.max >= subject.min);
        REQUIRE(subject.min == -1.0f);
        REQUIRE(subject.max == 2.0f);

        subject.expand(-2.0f);
        REQUIRE(subject.max >= subject.min);
        REQUIRE(subject.min == -2.0f);
        REQUIRE(subject.max == 2.0f);
    }

    SECTION("partialOverlap(const Range<T> &) should check if the current range overlaps with the supplied range") {
        const XE::Range<float> ranges[] = {
            XE::Range<float>{0.5f, 1.5f},
            XE::Range<float>{-1.5f, 0.5f},
            XE::Range<float>{0.25f, 1.5f},
            XE::Range<float>{-1.5f, 0.75f},
            XE::Range<float>{-10.5f, 0.1f},
            XE::Range<float>{-1.5f, 2.0f}
        };

        for (const auto &range : ranges) {
            REQUIRE(subject.partialOverlap(range));
        }
    }
}
