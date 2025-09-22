
#include <gtest/gtest.h>
#include <sstream>
#include <xe/math/Ellipsoid.h>

TEST(EllipsoidTest, DefaultConstructorInitializesAtTheOriginAndWithRadiousOne) {
    XE::Ellipsoid const e1;
    EXPECT_EQ(e1.center, XE::Vector3(0.0F, 0.0F, 0.0F));
    EXPECT_EQ(e1.size, XE::Vector3(1.0F, 1.0F, 1.0F));
}

TEST(EllipsoidTest, ConstructorInitializesAtTheSpecifiedOriginAndRadiouses) {
    XE::Ellipsoid const e1{{1.0F, 2.0F, -4.0F}, {4.0F, 2.0F, 1.0F}};
    EXPECT_EQ(e1.center, XE::Vector3(1.0F, 2.0F, -4.0F));
    EXPECT_EQ(e1.size, XE::Vector3(4.0F, 2.0F, 1.0F));
}

TEST(EllipsoidTest, CanBeSerializedToANonEmptyString) {
    XE::Ellipsoid const ellipsoid;

    std::stringstream ss;
    ss << ellipsoid;

    EXPECT_NE(ss.str(), "");
}
