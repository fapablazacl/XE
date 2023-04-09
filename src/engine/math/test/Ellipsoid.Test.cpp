
#include "xe/math/Ellipsoid.h"
#include "GoogleTestCommon.h"
#include <sstream>

TEST(EllipsoidTest, DefaultConstructorInitializesAtTheOriginAndWithRadiousOne) {
    XE::TEllipsoid<float> e1;
    EXPECT_EQ(e1.center, XE::Vector3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(e1.size, XE::Vector3(1.0f, 1.0f, 1.0f));
}

TEST(EllipsoidTest, ConstructorInitializesAtTheSpecifiedOriginAndRadiouses) {
    XE::TEllipsoid<float> e1{{1.0f, 2.0f, -4.0f}, {4.0f, 2.0f, 1.0f}};
    EXPECT_EQ(e1.center, XE::Vector3(1.0f, 2.0f, -4.0f));
    EXPECT_EQ(e1.size, XE::Vector3(4.0f, 2.0f, 1.0f));
}

TEST(EllipsoidTest, CanBeSerializedToANonEmptyString) {
    XE::TEllipsoid<float> ellipsoid;

    std::stringstream ss;
    ss << ellipsoid;

    EXPECT_NE(ss.str(), "");
}
