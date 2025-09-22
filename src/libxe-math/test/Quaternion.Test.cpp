
#include "xe/math/Quaternion.h"
#include "xe/math/Common.h"
#include "xe/math/Vector.h"
#include <cmath>
#include <gtest/gtest.h>

TEST(QuaternionTest, VectorScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q2 = XE::Quat{{0.0F, 1.0F, 0.0F}, 1.0F};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q2.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q2.W, 1.0F);
}

TEST(QuaternionTest, FourScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quat{0.0F, 1.0F, 0.0F, 1.0F};

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 1.0F);
}

TEST(QuaternionTest, OneScalarConstructorInitializesTheScalarPart) {
    const auto q2 = XE::Quat{1.0F};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q2.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q2.W, 1.0F);
}

TEST(QuaternionTest, Vector3ConstructorConstructorInitializesTheVectorPart) {
    const auto q = XE::Quat{{0.0F, 1.0F, 0.0F}};

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 0.0F);
}

TEST(QuaternionTest, Vector4ConstructorConstructorInitializesTheVectorPart) {
    const auto v = XE::Vector4{0.0F, 1.0F, 0.0F, 10.0F};
    const auto q = XE::Quat{v};

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 10.0F);
}

TEST(QuaternionTest, ThreeScalarConstructorInitializesTheVectorPart) {
    const auto q = XE::Quat{0.0F, 1.0F, 0.0F};

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 0.0F);
}

TEST(QuaternionTest, PointerConstructorInitializesTheVectorAndScalarPart) {
    const float values[] = {4.0F, 2.0F, 3.0F, 1.0F};

    const auto q = XE::Quat{values};

    EXPECT_FLOAT_EQ(q.V.X, 4.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0F);
    EXPECT_FLOAT_EQ(q.W, 1.0F);
}

TEST(QuaternionTest, CopyConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quat{XE::Quat{1.0F, 2.0F, 3.0F, 1.0F}};

    EXPECT_FLOAT_EQ(q.V.X, 1.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0F);
    EXPECT_FLOAT_EQ(q.W, 1.0F);
}

TEST(QuaternionTest, DataShouldReturnAPointerToTheFirstElement) {
    const auto quat = XE::Quat{0.0F, 1.0F, 2.0F, 3.0F};
    const float *data = quat.data();

    EXPECT_EQ(data[0], 0.0F);
    EXPECT_EQ(data[1], 1.0F);
    EXPECT_EQ(data[2], 2.0F);
    EXPECT_EQ(data[3], 3.0F);
}

TEST(QuaternionTest, SizeAlwaysReturnsFour) {
    const auto quat = XE::Quat{0.0F, 1.0F, 2.0F, 3.0F};
    EXPECT_EQ(quat.size(), 4);
}

TEST(QuaternionTest, OperatorBracketsShouldEnableAccessToEachElement) {
    auto quat = XE::Quat{0.0F, 1.0F, 2.0F, 3.0F};
    EXPECT_EQ(quat[0], 0.0F);
    EXPECT_EQ(quat[1], 1.0F);
    EXPECT_EQ(quat[2], 2.0F);
    EXPECT_EQ(quat[3], 3.0F);

    quat[3] = 10.0F;
    EXPECT_EQ(quat[3], 10.0F);

    const auto quat2 = XE::Quat{10.0F, -1.0F, -2.0F, -3.0F};
    EXPECT_EQ(quat2[0], 10.0F);
    EXPECT_EQ(quat2[1], -1.0F);
    EXPECT_EQ(quat2[2], -2.0F);
    EXPECT_EQ(quat2[3], -3.0F);
}

TEST(QuaternionTest, EqualityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quat{{1.0F, 2.0F, 3.0F}, 1.0F};
    const auto qa_2 = XE::Quat{1.0F, 2.0F, 3.0F, 1.0F};
    const auto qb_1 = XE::Quat{{1.0F, 2.0F, 3.0F}};
    const auto qb_2 = XE::Quat{1.0F, 2.0F, 3.0F};

    EXPECT_EQ(qa_1, qa_2);
    EXPECT_EQ(qb_1, qb_2);
    EXPECT_EQ(qa_2, qa_1);
    EXPECT_EQ(qb_2, qb_1);
}

TEST(QuaternionTest, InequalityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quat{{1.0F, 2.0F, 3.0F}, 1.0F};
    const auto qa_2 = XE::Quat{1.0F, 2.0F, 3.0F, 1.0F};
    const auto qb_1 = XE::Quat{{1.0F, 2.0F, 3.0F}};
    const auto qb_2 = XE::Quat{1.0F, 2.0F, 3.0F};

    EXPECT_NE(qa_1, qb_2);
    EXPECT_NE(qa_2, qb_1);

    EXPECT_NE(qb_1, qa_2);
    EXPECT_NE(qb_2, qa_1);
}

TEST(QuaternionTest, ZeroQuaternionFactoryMethodInitializesScalarAndVectorPartToZeroes) {
    const auto q = XE::quatZero<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 0.0F);
}

TEST(QuaternionTest, IdentityQuaternionFactoryMethodInitializesScalarPartToOne) {
    const auto q = XE::quatId<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0F);
    EXPECT_FLOAT_EQ(q.W, 1.0F);
}

TEST(QuaternionTest, RotationRHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine) {
    const float radians = XE::pi<float>;
    const XE::Vector3 axis{0.0F, 1.0F, 0.0F};

    const auto subject = XE::quatRotationRH<float>(axis, radians);
    const auto correct = XE::normalize(XE::Quat{axis * std::sin(radians * 0.5F), std::cos(radians * 0.5F)});

    EXPECT_FLOAT_EQ(XE::norm(subject), 1.0F);
    EXPECT_FLOAT_EQ(subject.V.X, correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST(QuaternionTest, RotationLHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine) {
    const float radians = XE::pi<float>;
    const XE::Vector3 axis{0.0F, 1.0F, 0.0F};

    const auto subject = XE::quatRotationLH<float>(axis, radians);
    const auto correct = XE::normalize(XE::Quat{axis * std::sin(radians * 0.5F), std::cos(radians * 0.5F)});

    EXPECT_FLOAT_EQ(subject.V.X, -correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, -correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, -correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST(QuaternionTest, DotShouldComputeThwSumOfProductsElementWise) {
    const XE::Quat v1 = {2.0F, 8.0F, 32.0F, 0.0F};
    const XE::Quat v2 = {1.0F, 2.0F, 4.0F, 0.0F};

    EXPECT_FLOAT_EQ(dot(v1, v2), 146.0F);
    EXPECT_FLOAT_EQ(dot(v2, v1), 146.0F);
}

TEST(QuaternionTest, Norm2ShouldComputeTheMagnitudeSquared) {
    const auto q = XE::Quat{2.0F, 3.0F, 4.0F, 5.0F};
    EXPECT_EQ(norm2(q), 54.0F);
}

TEST(QuaternionTest, NormShouldComputeTheMagnitude) {
    const auto q = XE::Quat{0.0F, 3.0F, 0.0F, 4.0F};
    EXPECT_EQ(norm(q), 5.0F);
}

TEST(QuaternionTest, ConjugateShouldNegateTheVectorPart) {
    const auto q4 = XE::conjugate(XE::Quat{{-1.0F, 1.0F, -1.0F}, 1.0F});

    EXPECT_EQ(q4.V.X, 1.0F);
    EXPECT_EQ(q4.V.Y, -1.0F);
    EXPECT_EQ(q4.V.Z, 1.0F);
    EXPECT_EQ(q4.W, 1.0F);
}

TEST(QuaternionTest, NormalizeShouldScaleAQuaternionToTheUnitLength) {
    const auto q1 = XE::normalize(XE::Quat{{1.0F, 1.0F, 1.0F}, 1.0F});
    EXPECT_EQ(q1.V.X, 0.5F);
    EXPECT_EQ(q1.V.Y, 0.5F);
    EXPECT_EQ(q1.V.Z, 0.5F);
    EXPECT_EQ(q1.W, 0.5F);

    const auto q2 = XE::normalize(XE::Quat{{0.0F, 4.0F, 0.0F}, 0.0F});
    EXPECT_EQ(q2.V.X, 0.0F);
    EXPECT_EQ(q2.V.Y, 1.0F);
    EXPECT_EQ(q2.V.Z, 0.0F);
    EXPECT_EQ(q2.W, 0.0F);

    const auto q3 = XE::normalize(XE::Quat{{0.0F, -1.0F, 0.0F}, 0.0F});
    EXPECT_EQ(q3.V.X, 0.0F);
    EXPECT_EQ(q3.V.Y, -1.0F);
    EXPECT_EQ(q3.V.Z, 0.0F);
    EXPECT_EQ(q3.W, 0.0F);
}

TEST(QuaternionTest, AdditionOperatorShouldAddTheirEachComponentsTogether) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(q1 + q1, XE::Quat(2.0F, 0.0F, 2.0F, 2.0F));
    EXPECT_EQ(q1 + q2, XE::Quat(1.0F, 1.0F, 1.0F, 2.0F));
}

TEST(QuaternionTest, SubtractionOperatorShouldSubtractEachComponentTogether) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(q1 - q1, XE::Quat(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(q1 - q2, XE::Quat(1.0F, -1.0F, 1.0F, 0.0F));
}

TEST(QuaternionTest, NegationOperatorShouldInvertEachComponent) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(-q1, XE::Quat(-1.0F, -0.0F, -1.0F, -1.0F));
    EXPECT_EQ(-q2, XE::Quat(0.0F, -1.0F, 0.0F, -1.0F));
}

TEST(QuaternionTest, PlusOperatorShouldModifyNoComponents) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(+q1, XE::Quat(1.0F, 0.0F, 1.0F, 1.0F));
    EXPECT_EQ(+q2, XE::Quat(0.0F, 1.0F, 0.0F, 1.0F));
}

TEST(QuaternionTest, MultiplyByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(q1 * 1.0F, XE::Quat(1.0F, 0.0F, 1.0F, 1.0F));
    EXPECT_EQ(q2 * 0.0F, XE::Quat(0.0F, 0.0F, 0.0F, 0.0F));
}

TEST(QuaternionTest, ScalarByQuaternionOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(1.0F * q1, XE::Quat(1.0F, 0.0F, 1.0F, 1.0F));
    EXPECT_EQ(0.0F * q2, XE::Quat(0.0F, 0.0F, 0.0F, 0.0F));
}

TEST(QuaternionTest, DivideByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_EQ(q1 / 1.0F, XE::Quat(1.0F, 0.0F, 1.0F, 1.0F));
    EXPECT_EQ(q2 / 0.5F, XE::Quat(0.0F, 2.0F, 0.0F, 2.0F));
}

TEST(QuaternionTest, MultiplyShouldCombineTwoQuaternions) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 0.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    const auto r1 = q1 * q2;
    EXPECT_FLOAT_EQ(r1.V.X, 1.0F);
    EXPECT_FLOAT_EQ(r1.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(r1.V.Z, 1.0F);
    EXPECT_FLOAT_EQ(r1.W, 1.0F);

    const auto r2 = q2 * q1;
    EXPECT_FLOAT_EQ(r2.V.X, 1.0F);
    EXPECT_FLOAT_EQ(r2.V.Y, 1.0F);
    EXPECT_FLOAT_EQ(r2.V.Z, -1.0F);
    EXPECT_FLOAT_EQ(r2.W, 1.0F);
}

TEST(QuaternionTest, MultiplyByIdentityQShouldNotHaveAnyAffect) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);
    const auto qi = XE::quatId<float>();

    EXPECT_EQ(q1 * qi, q1);
    EXPECT_EQ(q2 * qi, q2);
}

TEST(QuaternionTest, MultiplyByZeroQShouldCollapseAnyQuaternionToZero) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);
    const auto qz = XE::quatZero<float>();

    EXPECT_EQ(q1 * qz, qz);
    EXPECT_EQ(q2 * qz, qz);
}

TEST(QuaternionTest, DivideByIdentityShouldNotAffectAnyQuaternion) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);
    const auto qi = XE::quatId<float>();

    EXPECT_EQ(q1 / qi, q1);
    EXPECT_EQ(q2 / qi, q2);
}

TEST(QuaternionTest, DivideByZeroShouldCollapseAnyQuaternionToNaNValues) {
    const auto q1 = XE::Quat(1.0F, 0.0F, 1.0F, 1.0F);
    const auto q2 = XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);
    const auto qz = XE::quatZero<float>();

    const auto r1 = q1 / qz;
    const auto r2 = q2 / qz;

    EXPECT_TRUE(std::isnan(r1.V.X));
    EXPECT_TRUE(std::isnan(r1.V.Y));
    EXPECT_TRUE(std::isnan(r1.V.Z));
    EXPECT_TRUE(std::isnan(r1.W));

    EXPECT_TRUE(std::isnan(r2.V.X));
    EXPECT_TRUE(std::isnan(r2.V.Y));
    EXPECT_TRUE(std::isnan(r2.V.Z));
    EXPECT_TRUE(std::isnan(r2.W));
}

TEST(QuaternionTest, AddAndAssignOperatorShouldAddTheirEachComponentsTogether) {
    XE::Quat q1{{1.0F, 0.0F, 1.0F}, 1.0F};
    q1 += q1;

    EXPECT_FLOAT_EQ(q1.V.X, 2.0F);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q1.V.Z, 2.0F);
    EXPECT_FLOAT_EQ(q1.W, 2.0F);
}

TEST(QuaternionTest, SubtractractAndAssignOperatorShouldSubtractEachComponentTogether) {
    XE::Quat q1{{1.0F, 0.0F, 1.0F}, 1.0F};
    q1 -= XE::Quat(0.0F, 1.0F, 0.0F, 1.0F);

    EXPECT_FLOAT_EQ(q1.V.X, 1.0F);
    EXPECT_FLOAT_EQ(q1.V.Y, -1.0F);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0F);
    EXPECT_FLOAT_EQ(q1.W, 0.0F);
}

TEST(QuaternionTest, MultiplyAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quat q1{{1.0F, 0.0F, 1.0F}, 1.0F};

    q1 *= 3.0F;

    EXPECT_FLOAT_EQ(q1.V.X, 3.0F);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q1.V.Z, 3.0F);
    EXPECT_FLOAT_EQ(q1.W, 3.0F);
}

TEST(QuaternionTest, DivideAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quat q1{{1.0F, 0.0F, 1.0F}, 1.0F};

    q1 /= 1.0F;

    EXPECT_FLOAT_EQ(q1.V.X, 1.0F);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0F);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0F);
    EXPECT_FLOAT_EQ(q1.W, 1.0F);
}

TEST(QuaternionTest, Norm2ShouldComputeSquaredMagnitude) {
    const auto q = XE::Quat{3.0F, 4.0F, 5.0F, 0.0F};

    EXPECT_FLOAT_EQ(XE::norm2(q), 50.0F);
}

TEST(QuaternionTest, NormShouldComputeMagnitude) {
    const auto q = XE::Quat{3.0F, 4.0F, 5.0F, 0.0F};

    EXPECT_FLOAT_EQ(XE::norm(q), 7.071067812F);
}

TEST(QuaternionTest, ConjugateShouldReturnNegatedVectorPart) {
    const auto q = XE::Quat{3.0F, 4.0F, 5.0F, 1.0F};
    const auto conjugate = XE::conjugate(q);

    EXPECT_FLOAT_EQ(conjugate.V.X, -3.0F);
    EXPECT_FLOAT_EQ(conjugate.V.Y, -4.0F);
    EXPECT_FLOAT_EQ(conjugate.V.Z, -5.0F);
    EXPECT_FLOAT_EQ(conjugate.W, 1.0F);
}

TEST(QuaternionTest, NormalizeShouldReturnUnitLengthQuaternion) {
    const auto q = XE::Quat{3.0F, 4.0F, 5.0F, 0.0F};
    const auto normalized = XE::normalize(q);

    EXPECT_FLOAT_EQ(XE::norm(normalized), 1.0F);
}

TEST(QuaternionTest, InverseShouldComputeAnNormalizedCongujatedQuaternion) {
    const auto q1 = XE::Quat{{3.0F, 4.0F, 5.0F}, 0.0F};
    const auto inverse = XE::inverse(q1);

    EXPECT_FLOAT_EQ(inverse.V.X, -3.0F / 50.0F);
    EXPECT_FLOAT_EQ(inverse.V.Y, -4.0F / 50.0F);
    EXPECT_FLOAT_EQ(inverse.V.Z, -5.0F / 50.0F);
    EXPECT_FLOAT_EQ(inverse.W, 0.0F);
}

TEST(QuaternionTest, TransformShouldRotatePointVector) {
    const auto rotation = XE::quatRotationRH({0.0F, 1.0F, 0.0F}, XE::pi<float> * 0.5F);
    const auto point = XE::Vector3{1.0F, 0.0F, 0.0F};
    const auto result = XE::transform(rotation, point);
    const auto e = XE::DEFAULT_EPSILON<float>;

    EXPECT_NEAR(result.X, 0.0F, e);
    EXPECT_NEAR(result.Y, 0.0F, e);
    EXPECT_NEAR(result.Z, -1.0F, e);
}
