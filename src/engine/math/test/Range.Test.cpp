
#include "xe/math/Range.h"
#include "GoogleTestCommon.h"
#include <iostream>

TEST(RangeTest, DefaultConstructorShouldLetTheRangeWithValues0And1) {
    XE::TRange<float> subject;
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, 0.0f);
    EXPECT_EQ(subject.max, 1.0f);
}

TEST(RangeTest, TwoParamConstructorShouldLetTheRangeWithThoseValuesCorrectlySetted) {
    XE::TRange<float> subject = XE::TRange<float>{1.0f, -1.0f};

    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0f);
    EXPECT_EQ(subject.max, 1.0f);
}

TEST(RangeTest, ExpandShouldNotMutateTheRangeIfTheSuppliedValueIsInsideTheRange) {
    XE::TRange<float> subject;

    const float values[] = {0.0f, 0.25f, 0.5f, 0.75f, 1.0f};

    for (const float value : values) {
        subject.expand(value);
        EXPECT_GE(subject.max, subject.min);
        EXPECT_EQ(subject.min, 0.0f);
        EXPECT_EQ(subject.max, 1.0f);
    }
}

TEST(RangeTest, ExpandShouldMutateTheRangeIfTheSuppliedValueIsOutsideTheRange) {
    XE::TRange<float> subject;

    subject.expand(-1.0f);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0f);
    EXPECT_EQ(subject.max, 1.0f);

    subject.expand(2.0f);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0f);
    EXPECT_EQ(subject.max, 2.0f);

    subject.expand(-2.0f);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -2.0f);
    EXPECT_EQ(subject.max, 2.0f);
}

TEST(RangeTest, PartialOverlapShouldReturnTrueWhenOverlapsWithTheSuppliedRange) {
    XE::TRange<float> subject;

    const XE::TRange<float> ranges[] = {XE::TRange<float>{0.5f, 1.5f},   XE::TRange<float>{-1.5f, 0.5f},  XE::TRange<float>{0.25f, 1.5f},
                                        XE::TRange<float>{-1.5f, 0.75f}, XE::TRange<float>{-10.5f, 0.1f}, XE::TRange<float>{-1.5f, 2.0f}};

    for (const auto &range : ranges) {
        EXPECT_EQ(subject.partialOverlap(range), true);
    }
}

TEST(RangeTest, OverlapShouldReturnTrueWhenTheCurrentRangeOverlapsWithTheSuppliedRange) {
    XE::TRange<float> subject;

    const XE::TRange<float> ranges[] = {
        XE::TRange<float>{0.5f, 1.5f},  XE::TRange<float>{-1.5f, 0.5f}, XE::TRange<float>{0.25f, 1.5f}, XE::TRange<float>{-1.5f, 0.75f}, XE::TRange<float>{-10.5f, 0.1f},
        XE::TRange<float>{-1.5f, 2.0f}, XE::TRange<float>{-1.0f, 1.0f}, XE::TRange<float>{0.0f, 1.0f},  XE::TRange<float>{-2.0f, 2.0f},
    };

    for (const auto &range : ranges) {
        EXPECT_EQ(subject.overlap(range), true);
    }
}

TEST(RangeTest, OverlapShouldReturnFalseWhenUsingTightlyPositionedRanges) {
    for (int i = -10; i < 10; i++) {
        for (float offset = 0.0f; offset < 1.0f; offset += 0.125f) {
            const float width = 1.0f;

            const XE::TRange<float> subject1{(i + 0) * width + offset, (i + 1) * width + offset};

            const XE::TRange<float> subject2{(i + 1) * width + offset, (i + 2) * width + offset};

            EXPECT_EQ(subject1.partialOverlap(subject2), false);
        }
    }
}
