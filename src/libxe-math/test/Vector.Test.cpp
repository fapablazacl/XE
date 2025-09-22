

#include "xe/math/Vector.h"
#include <cmath>
#include <gtest/gtest.h>

TEST(VectorTest, DefaultConstructorShouldInitializeToZeroes) {
    const XE::Vector3 v;

    EXPECT_EQ(v.X, 0.0F);
    EXPECT_EQ(v.Y, 0.0F);
    EXPECT_EQ(v.Z, 0.0F);
}

TEST(VectorTest, ConstructorShouldSetupTheVectorComponentsCorrectly) {
    const XE::Vector3 v = {1.0F, 2.0F, 3.0F};

    EXPECT_EQ(v.X, 1.0F);
    EXPECT_EQ(v.Y, 2.0F);
    EXPECT_EQ(v.Z, 3.0F);

    EXPECT_EQ(v.values[0], 1.0F);
    EXPECT_EQ(v.values[1], 2.0F);
    EXPECT_EQ(v.values[2], 3.0F);

    EXPECT_EQ(v[0], 1.0F);
    EXPECT_EQ(v[1], 2.0F);
    EXPECT_EQ(v[2], 3.0F);
}

TEST(VectorTest, ConstructorFromPointerShouldSetupTheVectorComponentsCorrectly) {
    const float data[] = {1.0F, 2.0F, 3.0};
    const XE::Vector3 v{data};

    EXPECT_EQ(v.X, 1.0F);
    EXPECT_EQ(v.Y, 2.0F);
    EXPECT_EQ(v.Z, 3.0F);
}

TEST(VectorTest, DataMethodShouldReturnAnAddressToTheFirstComponent) {
    XE::Vector3 varv;
    EXPECT_NE(varv.data(), nullptr);
    EXPECT_EQ(varv.data(), &varv.values[0]);
}

TEST(VectorTest, ConstDataMethodShouldReturnAnAddressToTheFirstComponent) {
    const XE::Vector3 constv;
    EXPECT_NE(constv.data(), nullptr);
    EXPECT_EQ(constv.data(), &constv.values[0]);
}

TEST(VectorTest, CastMethodShouldConvertUnderlyingType) {
    const XE::Vector3 vf{1.0F, 2.0F, 3.0F};
    const auto vd = vf.cast<double>();

    EXPECT_EQ(vd.X, 1.0);
    EXPECT_EQ(vd.Y, 2.0);
    EXPECT_EQ(vd.Z, 3.0);
}

TEST(VectorTest, ComparisonOperatorsShouldCheckVectorComponentsForEqualityAndInequality) {
    const XE::Vector3 v1 = {1.0F, 2.0F, 3.0F};
    const XE::Vector3 v2 = {1.0F, 2.0F, 3.0F};
    const XE::Vector3 v3 = {-1.0F, -2.0F, -3.0F};

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
    EXPECT_NE(v2, v3);

    EXPECT_EQ(v2, v1);
    EXPECT_NE(v3, v1);
    EXPECT_NE(v3, v2);
}

TEST(VectorTest, OperatorPlusShouldAddComponentWise) {
    const XE::Vector3 v1{1.0F, -2.0F, 3.0F};
    const XE::Vector3 v2{-2.0F, -1.0F, -2.0F};

    EXPECT_EQ((v2 + v1), XE::Vector3(-1.0F, -3.0F, 1.0F));
    EXPECT_EQ((v1 + v2), XE::Vector3(-1.0F, -3.0F, 1.0F));
    EXPECT_EQ(v1, +v1);
    EXPECT_EQ(v2, +v2);
    EXPECT_EQ((XE::Vector3(0.0F) += v1), v1);
    EXPECT_EQ((XE::Vector3(0.0F) += v2), v2);
}

TEST(VectorTest, OperatorMinusShouldSubtractComponentWise) {
    const XE::Vector3 v1{1.0F, -2.0F, 3.0F};
    const XE::Vector3 v2{-2.0F, -1.0F, -2.0F};

    EXPECT_EQ((v2 - v1), XE::Vector3(-3.0F, 1.0F, -5.0F));
    EXPECT_EQ((v1 - v2), XE::Vector3(3.0F, -1.0F, 5.0F));
    EXPECT_EQ((v1 - v1), XE::Vector3(0.0F));
    EXPECT_EQ((v2 - v2), XE::Vector3(0.0F));

    EXPECT_EQ(XE::Vector3(-1.0F, 2.0F, -3.0F), -v1);
    EXPECT_EQ(XE::Vector3(2.0F, 1.0F, 2.0F), -v2);

    EXPECT_EQ((XE::Vector3(0.0F) -= v1), -v1);
    EXPECT_EQ((XE::Vector3(0.0F) -= v2), -v2);
}

TEST(VectorTest, OperatorMulShouldMultiplyComponentWise) {
    const XE::Vector3 v1 = {1.0F, -2.0F, 3.0F};
    const XE::Vector3 v2 = {-2.0F, -1.0F, -2.0F};

    EXPECT_EQ((v1 * v2), XE::Vector3(-2.0F, 2.0F, -6.0F));
    EXPECT_EQ((v2 * v1), XE::Vector3(-2.0F, 2.0F, -6.0F));

    EXPECT_EQ((v1 * 1.0F), v1);
    EXPECT_EQ((v1 * -1.0F), -v1);
    EXPECT_EQ((v1 * 2.0F), XE::Vector3(2.0F, -4.0F, 6.0F));
    EXPECT_EQ((v2 * 1.0F), v2);
    EXPECT_EQ((v2 * -1.0F), -v2);
    EXPECT_EQ((v2 * 2.0F), XE::Vector3(-4.0F, -2.0F, -4.0F));

    EXPECT_EQ((1.0F * v2), v2);
    EXPECT_EQ((-1.0F * v2), -v2);
    EXPECT_EQ((1.0F * v1), v1);
    EXPECT_EQ((-1.0F * v1), -v1);

    EXPECT_EQ((1.4F * v1), (v1 * 1.4F));
    EXPECT_EQ((1.4F * v2), (v2 * 1.4F));

    EXPECT_EQ((XE::Vector3(1.0F) *= v1), v1);
    EXPECT_EQ((XE::Vector3(1.0F) *= v2), v2);
}

TEST(VectorTest, OperatorMulAndAssignByScalarShouldMultiplyComponentWise) {
    XE::Vector3 v1 = {1.0F, -2.0F, 3.0F};

    v1 *= -1.0F;

    EXPECT_EQ(v1.X, -1.0F);
    EXPECT_EQ(v1.Y, 2.0F);
    EXPECT_EQ(v1.Z, -3.0F);
}

TEST(VectorTest, OperatorDivAndAssignByScalarShouldMultiplyComponentWise) {
    XE::Vector3 v1 = {2.0F, -2.0F, 4.0F};

    v1 /= 2.0F;

    EXPECT_EQ(v1.X, 1.0F);
    EXPECT_EQ(v1.Y, -1.0F);
    EXPECT_EQ(v1.Z, 2.0F);
}

TEST(VectorTest, OperatorDivShouldDivideComponentWise) {
    const XE::Vector3 v1{2.0F, 8.0F, 32.0F};
    const XE::Vector3 v2{1.0F, 2.0F, 4.0F};

    EXPECT_EQ((v1 / v2), XE::Vector3(2.0F, 4.0F, 8.0F));
    EXPECT_EQ((v1 / 2.0F), XE::Vector3(1.0F, 4.0F, 16.0F));
    EXPECT_EQ((v2 / 2.0F), XE::Vector3(0.5F, 1.0F, 2.0F));

    EXPECT_EQ((XE::Vector3(v1) /= v1), XE::Vector3(1.0F, 1.0F, 1.0F));
    EXPECT_EQ((XE::Vector3(v2) /= v2), XE::Vector3(1.0F, 1.0F, 1.0F));
}

TEST(VectorTest, DotProductShouldComputeASumOfProductsOfEachComponent) {
    const XE::Vector3 v1 = {2.0F, 8.0F, 32.0F};
    const XE::Vector3 v2 = {1.0F, 2.0F, 4.0F};

    EXPECT_FLOAT_EQ(dot(XE::Vector3(1.0F, 1.0F, 1.0F), v1), 42.0F);
    EXPECT_FLOAT_EQ(dot(XE::Vector3(1.0F, 1.0F, 1.0F), v2), 7.0F);
    EXPECT_FLOAT_EQ(dot(XE::Vector3(0.0F), v1), 0.0F);
    EXPECT_FLOAT_EQ(dot(XE::Vector3(0.0F), v2), 0.0F);

    EXPECT_FLOAT_EQ(dot(v1, XE::Vector3(1.0F, 1.0F, 1.0F)), 42.0F);
    EXPECT_FLOAT_EQ(dot(v2, XE::Vector3(1.0F, 1.0F, 1.0F)), 7.0F);
    EXPECT_FLOAT_EQ(dot(v1, XE::Vector3(0.0F)), 0.0F);
    EXPECT_FLOAT_EQ(dot(v2, XE::Vector3(0.0F)), 0.0F);

    EXPECT_FLOAT_EQ(dot(v1, v2), 146.0F);
    EXPECT_FLOAT_EQ(dot(v2, v1), 146.0F);
}

TEST(VectorTest, TripleDotProductFromUnitAxisComputesOne) {
    const XE::Vector3 v1 = {1.0F, 0.0F, 0.0F};
    const XE::Vector3 v2 = {0.0F, 1.0F, 0.0F};
    const XE::Vector3 v3 = {0.0F, 0.0F, 1.0F};

    const float result = XE::dot(v1, v2, v3);

    EXPECT_FLOAT_EQ(result, 1.0F);
}

TEST(VectorTest, TripleDotProductFromCoplanarVectorComputesZero) {
    const XE::Vector3 v1 = {1.0F, 0.0F, 0.0F};
    const XE::Vector3 v2 = {0.0F, 1.0F, 0.0F};
    const XE::Vector3 v3 = {-1.0F, -1.0F, 0.0F};

    EXPECT_FLOAT_EQ(XE::dot(v1, v2, v3), 0.0F);
}

TEST(VectorTest, TripleDotProductFromArbitraryAxesComputesSeven) {
    const XE::Vector3 v1 = {1.0F, -1.0F, 1.0F};
    const XE::Vector3 v2 = {2.0F, 1.0F, 1.0F};
    const XE::Vector3 v3 = {1.0F, 1.0F, -2.0F};

    const float result = XE::dot(v1, v2, v3);

    EXPECT_FLOAT_EQ(result, -7.0F);
}

TEST(VectorTest, TripleDotProductDontChangeFromCircularRotationOfParameters) {
    const XE::Vector3 v1 = {1.0F, -1.0F, 1.0F};
    const XE::Vector3 v2 = {2.0F, 1.0F, 1.0F};
    const XE::Vector3 v3 = {1.0F, 1.0F, -2.0F};

    EXPECT_FLOAT_EQ(XE::dot(v1, v2, v3), XE::dot(v3, v1, v2));
    EXPECT_FLOAT_EQ(XE::dot(v1, v2, v3), XE::dot(v2, v3, v1));
}

TEST(VectorTest, TripleDotProductChangesSignFromParameterSwapping) {
    const XE::Vector3 v1 = {1.0F, -1.0F, 1.0F};
    const XE::Vector3 v2 = {2.0F, 1.0F, 1.0F};
    const XE::Vector3 v3 = {1.0F, 1.0F, -2.0F};

    EXPECT_FLOAT_EQ(XE::dot(v1, v2, v3), -XE::dot(v2, v1, v3));
    EXPECT_FLOAT_EQ(XE::dot(v1, v2, v3), -XE::dot(v1, v3, v2));
}

TEST(VectorTest, TwoDimensionalCrossProductShouldReturnCrossVectorLength) {
    const XE::Vector2 v1 = {2.0F, 0.0F};
    const XE::Vector2 v2 = {0.0F, 2.0F};
    const XE::Vector2 v3 = {0.0F, -1.0F};

    EXPECT_EQ(cross(v1, v2), 4.0F);
    EXPECT_EQ(cross(v1, v3), -2.0F);
}

TEST(VectorTest, ThreeDimensionalCrossProductShouldPerformCorrectlyForUnitVectors) {
    const XE::Vector3 v1 = {1.0F, 0.0F, 0.0F};
    const XE::Vector3 v2 = {0.0F, 1.0F, 0.0F};
    const XE::Vector3 v3 = {0.0F, 0.0F, 1.0F};

    EXPECT_EQ(cross(v1, v2), v3);
    EXPECT_EQ(cross(v2, v1), -v3);

    EXPECT_EQ(cross(v2, v3), v1);
    EXPECT_EQ(cross(v3, v2), -v1);

    EXPECT_EQ(cross(v3, v1), v2);
    EXPECT_EQ(cross(v1, v3), -v2);

    EXPECT_EQ(cross(v1, v1), XE::Vector3(0.0F));
    EXPECT_EQ(cross(v2, v2), XE::Vector3(0.0F));
    EXPECT_EQ(cross(v3, v3), XE::Vector3(0.0F));
}

TEST(VectorTest, TripleCrossProductComputesACB_Minus_ABC_Product) {
    const XE::Vector3 v1 = {1.0F, 0.0F, 0.0F};
    const XE::Vector3 v2 = {0.0F, 1.0F, 0.0F};
    const XE::Vector3 v3 = {0.0F, 0.0F, 1.0F};

    EXPECT_EQ(cross(v1, v2, v3), dot(v1, v3) * v2 - dot(v1, v2) * v3);
}

TEST(VectorTest, MinimizeShouldReturnMinimunValuesBetweenTwoVectors) {
    const XE::Vector3 v1 = {1.0F, -2.0F, -1.3F};
    const XE::Vector3 v2 = {1.3F, -1.6F, 0.0F};
    const XE::Vector3 v3 = {-0.3F, 2.0F, 2.0F};

    EXPECT_EQ(minimize(v1, v1), v1);
    EXPECT_EQ(minimize(v2, v2), v2);
    EXPECT_EQ(minimize(v3, v3), v3);

    EXPECT_EQ(minimize(v1, v2), XE::Vector3(1.0F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v1, v3), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v2, v3), XE::Vector3(-0.3F, -1.6F, 0.0F));

    EXPECT_EQ(minimize(v2, v1), XE::Vector3(1.0F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v3, v1), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v3, v2), XE::Vector3(-0.3F, -1.6F, 0.0F));

    EXPECT_EQ(minimize(v1, minimize(v2, v3)), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v1, minimize(v3, v2)), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v2, minimize(v1, v3)), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v2, minimize(v3, v1)), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v3, minimize(v2, v1)), XE::Vector3(-0.3F, -2.0F, -1.3F));
    EXPECT_EQ(minimize(v3, minimize(v1, v2)), XE::Vector3(-0.3F, -2.0F, -1.3F));
}

TEST(VectorTest, MaximizeFunctionShouldReturnTheMaximunValuesBetweenTwoVectors) {
    const XE::Vector3 v1 = {1.0F, -2.0F, -1.3F};
    const XE::Vector3 v2 = {1.3F, -1.6F, 0.0F};
    const XE::Vector3 v3 = {-0.3F, 2.0F, 2.0F};

    EXPECT_EQ(maximize(v1, v1), v1);
    EXPECT_EQ(maximize(v2, v2), v2);
    EXPECT_EQ(maximize(v3, v3), v3);

    EXPECT_EQ(maximize(v1, v2), XE::Vector3(1.3F, -1.6F, 0.0F));
    EXPECT_EQ(maximize(v1, v3), XE::Vector3(1.0F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v2, v3), XE::Vector3(1.3F, 2.0F, 2.0F));

    EXPECT_EQ(maximize(v2, v1), XE::Vector3(1.3F, -1.6F, 0.0F));
    EXPECT_EQ(maximize(v3, v1), XE::Vector3(1.0F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v3, v2), XE::Vector3(1.3F, 2.0F, 2.0F));

    EXPECT_EQ(maximize(v1, maximize(v2, v3)), XE::Vector3(1.3F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v1, maximize(v3, v2)), XE::Vector3(1.3F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v2, maximize(v1, v3)), XE::Vector3(1.3F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v2, maximize(v3, v1)), XE::Vector3(1.3F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v3, maximize(v2, v1)), XE::Vector3(1.3F, 2.0F, 2.0F));
    EXPECT_EQ(maximize(v3, maximize(v1, v2)), XE::Vector3(1.3F, 2.0F, 2.0F));
}

TEST(VectorTest, Norm2FunctionShouldReturnAnSquaredVectorLength) {
    const XE::Vector3 v1 = {1.0F, -2.0F, -1.6F};
    const XE::Vector3 v2 = {1.3F, -1.6F, 0.0F};
    const XE::Vector3 v3 = {-0.3F, 2.0F, 2.0F};

    EXPECT_FLOAT_EQ(norm2(v1), 1.0F + 4.0F + 1.6F * 1.6F);
    EXPECT_FLOAT_EQ(norm2(v2), 1.3F * 1.3F + 1.6F * 1.6F);
    EXPECT_FLOAT_EQ(norm2(v3), 0.3F * 0.3F + 4.0F + 4.0F);
}

TEST(VectorTest, NormFunctionShouldReturnTheVectorLength) {
    const XE::Vector3 v1 = {0.0F, 0.0F, 0.0F};
    const XE::Vector3 v2 = {4.0F, 2.0F, 4.0F};
    const XE::Vector3 v3 = {1.0F, 1.0F, 1.0F};

    EXPECT_FLOAT_EQ(norm(v1), 0.0F);
    EXPECT_FLOAT_EQ(norm(v2), 6.0F);
    EXPECT_FLOAT_EQ(norm(v3), std::sqrt(3.0F));
}
