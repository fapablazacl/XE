
#include "Common.h"
#include <xe/math/Common.h>
#include <xe/math/Quaternion.h>

TEST(QuaternionTest, VectorScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q2 = XE::Quat{{0.0f, 1.0f, 0.0f}, 1.0f};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q2.W, 1.0f);
}

TEST(QuaternionTest, FourScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quat{0.0f, 1.0f, 0.0f, 1.0f};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, OneScalarConstructorInitializesTheScalarPart) {
    const auto q2 = XE::Quat{1.0f};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q2.W, 1.0f);
}

TEST(QuaternionTest, Vector3ConstructorConstructorInitializesTheVectorPart) {
    const auto q = XE::Quat{{0.0f, 1.0f, 0.0f}};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, Vector4ConstructorConstructorInitializesTheVectorPart) {
    const auto v = XE::Vector4{0.0f, 1.0f, 0.0f, 10.0f};
    const auto q = XE::Quat{v};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 10.0f);
}

TEST(QuaternionTest, ThreeScalarConstructorInitializesTheVectorPart) {
    const auto q = XE::Quat{0.0f, 1.0f, 0.0f};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, PointerConstructorInitializesTheVectorAndScalarPart) {
    const float values[] = {
        4.0f, 2.0f, 3.0f, 1.0f
    };

    const auto q = XE::Quat{values};

    EXPECT_FLOAT_EQ(q.V.X, 4.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, CopyConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quat{XE::Quat{1.0f, 2.0f, 3.0f, 1.0f}};

    EXPECT_FLOAT_EQ(q.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}




TEST(QuaternionTest, DataShouldReturnAPointerToTheFirstElement) {
    const auto quat = XE::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    const float *data = quat.data();

    EXPECT_EQ(data[0], 0.0f);
    EXPECT_EQ(data[1], 1.0f);
    EXPECT_EQ(data[2], 2.0f);
    EXPECT_EQ(data[3], 3.0f);
}


TEST(QuaternionTest, SizeAlwaysReturnsFour) {
    const auto quat = XE::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    EXPECT_EQ(quat.size(), 4);
}


TEST(QuaternionTest, OperatorBracketsShouldEnableAccessToEachElement) {
    auto quat = XE::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    EXPECT_EQ(quat[0], 0.0f);
    EXPECT_EQ(quat[1], 1.0f);
    EXPECT_EQ(quat[2], 2.0f);
    EXPECT_EQ(quat[3], 3.0f);

    quat[3] = 10.0f;
    EXPECT_EQ(quat[3], 10.0f);

    const auto quat2 = XE::Quat{10.0f, -1.0f, -2.0f, -3.0f};
    EXPECT_EQ(quat2[0], 10.0f);
    EXPECT_EQ(quat2[1], -1.0f);
    EXPECT_EQ(quat2[2], -2.0f);
    EXPECT_EQ(quat2[3], -3.0f);
}


TEST(QuaternionTest, EqualityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = XE::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = XE::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = XE::Quat{1.0f, 2.0f, 3.0f};

    EXPECT_EQ(qa_1, qa_2);
    EXPECT_EQ(qb_1, qb_2);
    EXPECT_EQ(qa_2, qa_1);
    EXPECT_EQ(qb_2, qb_1);
}

TEST(QuaternionTest, InequalityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = XE::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = XE::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = XE::Quat{1.0f, 2.0f, 3.0f};

    EXPECT_NE(qa_1, qb_2);
    EXPECT_NE(qa_2, qb_1);

    EXPECT_NE(qb_1, qa_2);
    EXPECT_NE(qb_2, qa_1);
}

TEST(QuaternionTest, ZeroQuaternionFactoryMethodInitializesScalarAndVectorPartToZeroes) {
    const auto q = XE::quatZero<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, IdentityQuaternionFactoryMethodInitializesScalarPartToOne) {
    const auto q = XE::quatId<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, RotationRHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine) {
    const float radians = XE::pi<float>;
    const XE::Vector3 axis {0.0f, 1.0f, 0.0f};

    const auto subject = XE::quatRotationRH<float>(axis, radians);
    const auto correct = XE::normalize(XE::Quat{ axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    EXPECT_FLOAT_EQ(XE::norm(subject), 1.0f);
    EXPECT_FLOAT_EQ(subject.V.X, correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST(QuaternionTest, RotationLHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine) {
    const float radians = XE::pi<float>;
    const XE::Vector3 axis {0.0f, 1.0f, 0.0f};

    const auto subject = XE::quatRotationLH<float>(axis, radians);
    const auto correct = XE::normalize(XE::Quat{ axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    EXPECT_FLOAT_EQ(subject.V.X, -correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, -correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, -correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST(QuaternionTest, DotShouldComputeThwSumOfProductsElementWise) {
    const XE::Quat v1 = {2.0f, 8.0f, 32.0f, 0.0f};
    const XE::Quat v2 = {1.0f, 2.0f, 4.0f, 0.0f};

    EXPECT_FLOAT_EQ(dot(v1, v2), 146.0f);
    EXPECT_FLOAT_EQ(dot(v2, v1), 146.0f);
}

TEST(QuaternionTest, Norm2ShouldComputeTheMagnitudeSquared) {
    const auto q = XE::Quat{2.0f, 3.0f, 4.0f, 5.0f};
    EXPECT_EQ(norm2(q), 54.0f);
}

TEST(QuaternionTest, NormShouldComputeTheMagnitude) {
    const auto q = XE::Quat{0.0f, 3.0f, 0.0f, 4.0f};
    EXPECT_EQ(norm(q), 5.0f);
}

TEST(QuaternionTest, ConjugateShouldNegateTheVectorPart) {
    const auto q4 = XE::conjugate(XE::Quat{{-1.0f, 1.0f, -1.0f}, 1.0f});

    EXPECT_EQ(q4.V.X, 1.0f);
    EXPECT_EQ(q4.V.Y, -1.0f);
    EXPECT_EQ(q4.V.Z, 1.0f);
    EXPECT_EQ(q4.W, 1.0f);
}

TEST(QuaternionTest, NormalizeShouldScaleAQuaternionToTheUnitLength) {
    const auto q1 = XE::normalize(XE::Quat{{1.0f, 1.0f, 1.0f}, 1.0f});
    EXPECT_EQ(q1.V.X, 0.5f);
    EXPECT_EQ(q1.V.Y, 0.5f);
    EXPECT_EQ(q1.V.Z, 0.5f);
    EXPECT_EQ(q1.W, 0.5f);

    const auto q2 = XE::normalize(XE::Quat{{0.0f, 4.0f, 0.0f}, 0.0f});
    EXPECT_EQ(q2.V.X, 0.0f);
    EXPECT_EQ(q2.V.Y, 1.0f);
    EXPECT_EQ(q2.V.Z, 0.0f);
    EXPECT_EQ(q2.W, 0.0f);

    const auto q3 = XE::normalize(XE::Quat{{0.0f, -1.0f, 0.0f}, 0.0f});
    EXPECT_EQ(q3.V.X, 0.0f);
    EXPECT_EQ(q3.V.Y, -1.0f);
    EXPECT_EQ(q3.V.Z, 0.0f);
    EXPECT_EQ(q3.W, 0.0f);
}

TEST(QuaternionTest, AdditionOperatorShouldAddTheirEachComponentsTogether) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 + q1, XE::Quat(2.0f, 0.0f, 2.0f, 2.0f));
    EXPECT_EQ(q1 + q2, XE::Quat(1.0f, 1.0f, 1.0f, 2.0f));
}

TEST(QuaternionTest, SubtractionOperatorShouldSubtractEachComponentTogether) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 - q1, XE::Quat(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(q1 - q2, XE::Quat(1.0f, -1.0f, 1.0f, 0.0f));
}

TEST(QuaternionTest, NegationOperatorShouldInvertEachComponent) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(-q1, XE::Quat(-1.0f, -0.0f, -1.0f, -1.0f));
    EXPECT_EQ(-q2, XE::Quat(0.0f, -1.0f, 0.0f, -1.0f));
}

TEST(QuaternionTest, PlusOperatorShouldModifyNoComponents) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(+q1, XE::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(+q2, XE::Quat(0.0f, 1.0f, 0.0f, 1.0f));
}

TEST(QuaternionTest, MultiplyByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 * 1.0f, XE::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(q2 * 0.0f, XE::Quat(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(QuaternionTest, ScalarByQuaternionOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(1.0f * q1, XE::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(0.0f * q2, XE::Quat(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(QuaternionTest, DivideByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 / 1.0f, XE::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(q2 / 0.5f, XE::Quat(0.0f, 2.0f, 0.0f, 2.0f));
}


TEST(QuaternionTest, MultiplyShouldCombineTwoQuaternions) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 0.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    const auto r1 = q1 * q2;
    EXPECT_FLOAT_EQ(r1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(r1.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(r1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(r1.W, 1.0f);

    const auto r2 = q2 * q1;
    EXPECT_FLOAT_EQ(r2.V.X, 1.0f);
    EXPECT_FLOAT_EQ(r2.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(r2.V.Z, -1.0f);
    EXPECT_FLOAT_EQ(r2.W, 1.0f);
}

TEST(QuaternionTest, MultiplyByIdentityQShouldNotHaveAnyAffect) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = XE::quatId<float>();

    EXPECT_EQ(q1 * qi, q1);
    EXPECT_EQ(q2 * qi, q2);
}

TEST(QuaternionTest, MultiplyByZeroQShouldCollapseAnyQuaternionToZero) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = XE::quatZero<float>();

    EXPECT_EQ(q1 * qz, qz);
    EXPECT_EQ(q2 * qz, qz);
}

TEST(QuaternionTest, DivideByIdentityShouldNotAffectAnyQuaternion) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = XE::quatId<float>();

    EXPECT_EQ(q1 / qi, q1);
    EXPECT_EQ(q2 / qi, q2);
}

TEST(QuaternionTest, DivideByZeroShouldCollapseAnyQuaternionToNaNValues) {
    const auto q1 = XE::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);
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
    XE::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 += q1;

    EXPECT_FLOAT_EQ(q1.V.X, 2.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 2.0f);
    EXPECT_FLOAT_EQ(q1.W, 2.0f);
}

TEST(QuaternionTest, SubtractractAndAssignOperatorShouldSubtractEachComponentTogether) {
    XE::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 -= XE::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, -1.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 0.0f);
}

TEST(QuaternionTest, MultiplyAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 *= 3.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 3.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q1.W, 3.0f);
}


TEST(QuaternionTest, DivideAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 /= 1.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 1.0f);
}


TEST(QuaternionTest, Norm2ShouldComputeSquaredMagnitude) {
    const auto q = XE::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(XE::norm2(q), 50.0f);
}


TEST(QuaternionTest, NormShouldComputeMagnitude) {
    const auto q = XE::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(XE::norm(q), 7.071067812f);
}


TEST(QuaternionTest, ConjugateShouldReturnNegatedVectorPart) {
    const auto q = XE::Quat{3.0f, 4.0f, 5.0f, 1.0f};
    const auto conjugate = XE::conjugate(q);

    EXPECT_FLOAT_EQ(conjugate.V.X, -3.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Y, -4.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Z, -5.0f);
    EXPECT_FLOAT_EQ(conjugate.W, 1.0f);
}


TEST(QuaternionTest, NormalizeShouldReturnUnitLengthQuaternion) {
    const auto q = XE::Quat{3.0f, 4.0f, 5.0f, 0.0f};
    const auto normalized = XE::normalize(q);

    EXPECT_FLOAT_EQ(XE::norm(normalized), 1.0f);
}


TEST(QuaternionTest, InverseShouldComputeAnNormalizedCongujatedQuaternion) {
    const auto q1 = XE::Quat{{3.0f, 4.0f, 5.0f}, 0.0f};
    const auto inverse = XE::inverse(q1);

    EXPECT_FLOAT_EQ(inverse.V.X, -3.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.V.Y, -4.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.V.Z, -5.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.W, 0.0f);
}


TEST(QuaternionTest, TransformShouldRotatePointVector) {
    const auto rotation= XE::quatRotationRH<float>({0.0f, 1.0f, 0.0f}, XE::pi<float> * 0.5);
    const auto point = XE::Vector3{1.0f, 0.0f, 0.0f};
    const auto result = XE::transform(rotation, point);

    EXPECT_FLOAT_EQ(result.X, 0.0f);
    EXPECT_FLOAT_EQ(result.Y, 0.0f);
    EXPECT_FLOAT_EQ(result.Z, -1.0f);
}
