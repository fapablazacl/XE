
#include "GoogleTestCommon.h"
#include <sstream>
#include <xe/math/Ellipsoid.h>

TEST(EllipsoidTest, DefaultConstructorInitializesAtTheOriginAndWithRadiousOne) {
    XE::Ellipsoid<float> e1;
    EXPECT_EQ(e1.center, XE::Vector3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(e1.size, XE::Vector3(1.0f, 1.0f, 1.0f));
}

TEST(EllipsoidTest, ConstructorInitializesAtTheSpecifiedOriginAndRadiouses) {
    XE::Ellipsoid<float> e1{{1.0f, 2.0f, -4.0f}, {4.0f, 2.0f, 1.0f}};
    EXPECT_EQ(e1.center, XE::Vector3(1.0f, 2.0f, -4.0f));
    EXPECT_EQ(e1.size, XE::Vector3(4.0f, 2.0f, 1.0f));
}

TEST(EllipsoidTest, CanBeSerializedToANonEmptyString) {
    XE::Ellipsoid<float> ellipsoid;

    std::stringstream ss;
    ss << ellipsoid;

    EXPECT_NE(ss.str(), "");
}
