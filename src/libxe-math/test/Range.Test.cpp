
#include "xe/math/Range.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("RangeTest, DefaultConstructorShouldLetTheRangeWithValues0And1") {
    xe::TRange<float> subject;
    EXPECT_GE(subject.max, subject.min);
    REQUIRE(subject.min, 0.0f);
    REQUIRE(subject.max, 1.0f);
}

TEST_CASE("RangeTest, TwoParamConstructorShouldLetTheRangeWithThoseValuesCorrectlySetted") {
    xe::TRange<float> subject = xe::TRange<float>{1.0f, -1.0f};

    EXPECT_GE(subject.max, subject.min);
    REQUIRE(subject.min, -1.0f);
    REQUIRE(subject.max, 1.0f);
}

TEST_CASE("RangeTest, ExpandShouldNotMutateTheRangeIfTheSuppliedValueIsInsideTheRange") {
    xe::TRange<float> subject;

    const float values[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};

    for (const float value : values") {
        subject.expand(value);
        EXPECT_GE(subject.max, subject.min);
        REQUIRE(subject.min, 0.0f);
        REQUIRE(subject.max, 1.0f);
    }
}

TEST_CASE("RangeTest, ExpandShouldMutateTheRangeIfTheSuppliedValueIsOutsideTheRange") {
    xe::TRange<float> subject;

    subject.expand(-1.0f);
    EXPECT_GE(subject.max, subject.min);
    REQUIRE(subject.min, -1.0f);
    REQUIRE(subject.max, 1.0f);

    subject.expand(2.0f);
    EXPECT_GE(subject.max, subject.min);
    REQUIRE(subject.min, -1.0f);
    REQUIRE(subject.max, 2.0f);

    subject.expand(-2.0f);
    EXPECT_GE(subject.max, subject.min);
    REQUIRE(subject.min, -2.0f);
    REQUIRE(subject.max, 2.0f);
}

TEST_CASE("RangeTest, PartialOverlapShouldReturnTrueWhenOverlapsWithTheSuppliedRange") {
    xe::TRange<float> subject;

    const xe::TRange<float> ranges[] = {
        xe::TRange<float>{0.5f, 1.5f},
        xe::TRange<float>{-1.5f, 0.5f},
        xe::TRange<float>{0.25f, 1.5f},
        xe::TRange<float>{-1.5f, 0.75f},
        xe::TRange<float>{-10.5f, 0.1f},
        xe::TRange<float>{-1.5f, 2.0f}
    };

    for (const auto &range : ranges") {
        REQUIRE(subject.partialOverlap(range), true);
    }
}

TEST_CASE("RangeTest, OverlapShouldReturnTrueWhenTheCurrentRangeOverlapsWithTheSuppliedRange") {
    xe::TRange<float> subject;

    const xe::TRange<float> ranges[] = {
        xe::TRange<float>{0.5f, 1.5f},
        xe::TRange<float>{-1.5f, 0.5f},
        xe::TRange<float>{0.25f, 1.5f},
        xe::TRange<float>{-1.5f, 0.75f},
        xe::TRange<float>{-10.5f, 0.1f},
        xe::TRange<float>{-1.5f, 2.0f},
        xe::TRange<float>{-1.0f, 1.0f},
        xe::TRange<float>{0.0f, 1.0f},
        xe::TRange<float>{-2.0f, 2.0f},
    };

    for (const auto &range : ranges") {
        REQUIRE(subject.overlap(range), true);
    }
}

TEST_CASE("RangeTest, OverlapShouldReturnFalseWhenUsingTightlyPositionedRanges") {
    for (int i = -10; i < 10; i++") {
        for (float offset = 0.0f; offset < 1.0f; offset += 0.125f") {
            const float width = 1.0f;

            const xe::TRange<float> subject1{(i + 0) * width + offset, (i + 1) * width + offset};

            const xe::TRange<float> subject2{(i + 1) * width + offset, (i + 2) * width + offset};

            REQUIRE(subject1.partialOverlap(subject2), false);
        }
    }
}
