
#include "xe/math/Common.h"
#include "GoogleTestCommon.h"

TEST(EqualsTest, WithNoEpsilonComparesExactly) {
    EXPECT_EQ(XE::equals(1.0f, 1.0f, 0.0f), true);
    EXPECT_EQ(XE::equals(1.0f, 1.0f), true);
}

TEST(EqualsTest, WithEpsilonComparesApproximetly) {
    EXPECT_EQ(XE::equals(1.00001f, 1.0f, 0.0001f), true);
    EXPECT_EQ(XE::equals(-1.00001f, -1.0f, 0.0001f), true);
    EXPECT_EQ(XE::equals(1.001f, 1.0f, 0.0001f), false);
    EXPECT_EQ(XE::equals(-1.001f, -1.0f, 0.0001f), false);
}
