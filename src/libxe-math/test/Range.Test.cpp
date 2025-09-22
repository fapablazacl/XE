
#include "xe/math/Range.h"
#include <gtest/gtest.h>

TEST(RangeTest, DefaultConstructorShouldLetTheRangeWithValues0And1) {
    XE::TRange<float> const subject;
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, 0.0F);
    EXPECT_EQ(subject.max, 1.0F);
}

TEST(RangeTest, TwoParamConstructorShouldLetTheRangeWithThoseValuesCorrectlySetted) {
    XE::TRange<float> const subject = XE::TRange<float>{1.0F, -1.0F};

    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0F);
    EXPECT_EQ(subject.max, 1.0F);
}

TEST(RangeTest, ExpandShouldNotMutateTheRangeIfTheSuppliedValueIsInsideTheRange) {
    XE::TRange<float> subject;

    const float values[] = {0.0F, 0.25F, 0.5F, 0.75F, 1.0F};

    for (const float value : values) {
        subject.expand(value);
        EXPECT_GE(subject.max, subject.min);
        EXPECT_EQ(subject.min, 0.0F);
        EXPECT_EQ(subject.max, 1.0F);
    }
}

TEST(RangeTest, ExpandShouldMutateTheRangeIfTheSuppliedValueIsOutsideTheRange) {
    XE::TRange<float> subject;

    subject.expand(-1.0F);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0F);
    EXPECT_EQ(subject.max, 1.0F);

    subject.expand(2.0F);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -1.0F);
    EXPECT_EQ(subject.max, 2.0F);

    subject.expand(-2.0F);
    EXPECT_GE(subject.max, subject.min);
    EXPECT_EQ(subject.min, -2.0F);
    EXPECT_EQ(subject.max, 2.0F);
}

TEST(RangeTest, PartialOverlapShouldReturnTrueWhenOverlapsWithTheSuppliedRange) {
    XE::TRange<float> const subject;

    const XE::TRange<float> ranges[] = {
        XE::TRange<float>{0.5F, 1.5F},
        XE::TRange<float>{-1.5F, 0.5F},
        XE::TRange<float>{0.25F, 1.5F},
        XE::TRange<float>{-1.5F, 0.75F},
        XE::TRange<float>{-10.5F, 0.1F},
        XE::TRange<float>{-1.5F, 2.0F}
    };

    for (const auto &range : ranges) {
        EXPECT_EQ(subject.partialOverlap(range), true);
    }
}

TEST(RangeTest, OverlapShouldReturnTrueWhenTheCurrentRangeOverlapsWithTheSuppliedRange) {
    XE::TRange<float> const subject;

    const XE::TRange<float> ranges[] = {
        XE::TRange<float>{0.5F, 1.5F},
        XE::TRange<float>{-1.5F, 0.5F},
        XE::TRange<float>{0.25F, 1.5F},
        XE::TRange<float>{-1.5F, 0.75F},
        XE::TRange<float>{-10.5F, 0.1F},
        XE::TRange<float>{-1.5F, 2.0F},
        XE::TRange<float>{-1.0F, 1.0F},
        XE::TRange<float>{0.0F, 1.0F},
        XE::TRange<float>{-2.0F, 2.0F},
    };

    for (const auto &range : ranges) {
        EXPECT_EQ(subject.overlap(range), true);
    }
}

TEST(RangeTest, OverlapShouldReturnFalseWhenUsingTightlyPositionedRanges) {
    for (int i = -10; i < 10; i++) {
        for (float offset = 0.0F; offset < 1.0F; offset += 0.125F) {
            const float width = 1.0F;

            const XE::TRange<float> subject1{((i + 0) * width) + offset, ((i + 1) * width) + offset};

            const XE::TRange<float> subject2{((i + 1) * width) + offset, ((i + 2) * width) + offset};

            EXPECT_EQ(subject1.partialOverlap(subject2), false);
        }
    }
}
