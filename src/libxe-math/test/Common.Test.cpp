
#include "xe/math/Common.h"
#include <gtest/gtest.h>

TEST(EqualsTest, WithNoEpsilonComparesExactly) {
    EXPECT_EQ(XE::equals(1.0F, 1.0F, 0.0F), true);
    EXPECT_EQ(XE::equals(1.0F, 1.0F), true);
}

TEST(EqualsTest, WithEpsilonComparesApproximetly) {
    EXPECT_EQ(XE::equals(1.00001F, 1.0F, 0.0001F), true);
    EXPECT_EQ(XE::equals(-1.00001F, -1.0F, 0.0001F), true);
    EXPECT_EQ(XE::equals(1.001F, 1.0F, 0.0001F), false);
    EXPECT_EQ(XE::equals(-1.001F, -1.0F, 0.0001F), false);
}
