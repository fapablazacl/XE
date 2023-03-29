
#include "Common.h"
#include <xe/math/Common.h>
#include <xe/math/FormatUtils.h>
#include <xe/math/Quaternion.h>

TEST(QuaternionTest, VectorScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q2 = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}, 1.0f};

    EXPECT_EQ(q2.V.X, 0.0f);
    EXPECT_EQ(q2.V.Y, 1.0f);
    EXPECT_EQ(q2.V.Z, 0.0f);
    EXPECT_EQ(q2.W, 1.0f);
}

TEST(QuaternionTest, FourScalarConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quaternion<float>{0.0f, 1.0f, 0.0f, 1.0f};

    EXPECT_EQ(q.V.X, 0.0f);
    EXPECT_EQ(q.V.Y, 1.0f);
    EXPECT_EQ(q.V.Z, 0.0f);
    EXPECT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, OneScalarConstructorInitializesTheScalarPart) {
    const auto q2 = XE::Quaternion<float>{1.0f};

    EXPECT_EQ(q2.V.X, 0.0f);
    EXPECT_EQ(q2.V.Y, 0.0f);
    EXPECT_EQ(q2.V.Z, 0.0f);
    EXPECT_EQ(q2.W, 1.0f);
}

TEST(QuaternionTest, VectorConstructorConstructorInitializesTheVectorPart) {
    const auto q = XE::Quaternion<float>{{0.0f, 1.0f, 0.0f}};

    EXPECT_EQ(q.V.X, 0.0f);
    EXPECT_EQ(q.V.Y, 1.0f);
    EXPECT_EQ(q.V.Z, 0.0f);
    EXPECT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, ThreeScalarConstructorInitializesTheVectorPart) {
    const auto q = XE::Quaternion<float>{0.0f, 1.0f, 0.0f};

    EXPECT_EQ(q.V.X, 0.0f);
    EXPECT_EQ(q.V.Y, 1.0f);
    EXPECT_EQ(q.V.Z, 0.0f);
    EXPECT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, PointerConstructorInitializesTheVectorAndScalarPart) {
    const float values[] = {
        4.0f, 2.0f, 3.0f, 1.0f
    };

    const auto q = XE::Quaternion<float>{values};

    EXPECT_EQ(q.V.X, 4.0f);
    EXPECT_EQ(q.V.Y, 2.0f);
    EXPECT_EQ(q.V.Z, 3.0f);
    EXPECT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, CopyConstructorInitializesTheVectorAndScalarPart) {
    const auto q = XE::Quaternion<float>{XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f}};

    EXPECT_EQ(q.V.X, 1.0f);
    EXPECT_EQ(q.V.Y, 2.0f);
    EXPECT_EQ(q.V.Z, 3.0f);
    EXPECT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, EqualityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f};

    EXPECT_EQ(qa_1, qa_2);
    EXPECT_EQ(qb_1, qb_2);
    EXPECT_EQ(qa_2, qa_1);
    EXPECT_EQ(qb_2, qb_1);
}

TEST(QuaternionTest, InequalityOperatorDoesElementWiseComparisonViaFPTolerance) {
    const auto qa_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = XE::Quaternion<float>{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = XE::Quaternion<float>{1.0f, 2.0f, 3.0f};

    EXPECT_NE(qa_1, qb_2);
    EXPECT_NE(qa_2, qb_1);

    EXPECT_NE(qb_1, qa_2);
    EXPECT_NE(qb_2, qa_1);
}

TEST(QuaternionTest, ZeroQuaternionFactoryMethodInitializesScalarAndVectorPartToZeroes) {
    const auto q = XE::Quaternion<float>::createZero();

    EXPECT_EQ(q.V.X, 0.0f);
    EXPECT_EQ(q.V.Y, 0.0f);
    EXPECT_EQ(q.V.Z, 0.0f);
    EXPECT_EQ(q.W, 0.0f);
}

TEST(QuaternionTest, IdentityQuaternionFactoryMethodInitializesScalarPartToOne) {
    const auto q = XE::Quaternion<float>::createIdentity();

    EXPECT_EQ(q.V.X, 0.0f);
    EXPECT_EQ(q.V.Y, 0.0f);
    EXPECT_EQ(q.V.Z, 0.0f);
    EXPECT_EQ(q.W, 1.0f);
}

TEST(QuaternionTest, DotShouldComputeThwSumOfProductsElementWise) {
    const XE::Quaternion<float> v1 = {2.0f, 8.0f, 32.0f, 0.0f};
    const XE::Quaternion<float> v2 = {1.0f, 2.0f, 4.0f, 0.0f};

    EXPECT_EQ(dot(v1, v2), 146.0f);
    EXPECT_EQ(dot(v2, v1), 146.0f);
}

TEST(QuaternionTest, Norm2ShouldComputeTheMagnitudeSquared) {
    const auto q = XE::Quaternion<float>{2.0f, 3.0f, 4.0f, 5.0f};
    EXPECT_EQ(norm2(q), 54.0f);
}

TEST(QuaternionTest, NormShouldComputeTheMagnitude) {
    const auto q = XE::Quaternion<float>{0.0f, 3.0f, 0.0f, 4.0f};
    EXPECT_EQ(norm(q), 5.0f);
}

TEST(QuaternionTest, ConjugateShouldNegateTheVectorPart) {
    const auto q4 = XE::conjugate(XE::Quaternion<float>{{-1.0f, 1.0f, -1.0f}, 1.0f});

    EXPECT_EQ(q4.V.X, 1.0f);
    EXPECT_EQ(q4.V.Y, -1.0f);
    EXPECT_EQ(q4.V.Z, 1.0f);
    EXPECT_EQ(q4.W, 1.0f);
}

TEST(QuaternionTest, NormalizeShouldScaleAQuaternionToTheUnitLength) {
    const auto q1 = XE::normalize(XE::Quaternion<float>{{1.0f, 1.0f, 1.0f}, 1.0f});
    EXPECT_EQ(q1.V.X, 0.5f);
    EXPECT_EQ(q1.V.Y, 0.5f);
    EXPECT_EQ(q1.V.Z, 0.5f);
    EXPECT_EQ(q1.W, 0.5f);

    const auto q2 = XE::normalize(XE::Quaternion<float>{{0.0f, 4.0f, 0.0f}, 0.0f});
    EXPECT_EQ(q2.V.X, 0.0f);
    EXPECT_EQ(q2.V.Y, 1.0f);
    EXPECT_EQ(q2.V.Z, 0.0f);
    EXPECT_EQ(q2.W, 0.0f);

    const auto q3 = XE::normalize(XE::Quaternion<float>{{0.0f, -1.0f, 0.0f}, 0.0f});
    EXPECT_EQ(q3.V.X, 0.0f);
    EXPECT_EQ(q3.V.Y, -1.0f);
    EXPECT_EQ(q3.V.Z, 0.0f);
    EXPECT_EQ(q3.W, 0.0f);
}

TEST(QuaternionTest, AdditionOperatorShouldAddTheirEachComponentsTogether) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 + q1, XE::Quaternion<float>(2.0f, 0.0f, 2.0f, 2.0f));
    EXPECT_EQ(q1 + q2, XE::Quaternion<float>(1.0f, 1.0f, 1.0f, 2.0f));
}

TEST(QuaternionTest, SubtractionOperatorShouldSubtractEachComponentTogether) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 - q1, XE::Quaternion<float>(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(q1 - q2, XE::Quaternion<float>(1.0f, -1.0f, 1.0f, 0.0f));
}

TEST(QuaternionTest, NegationOperatorShouldInvertEachComponent) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(-q1, XE::Quaternion<float>(-1.0f, -0.0f, -1.0f, -1.0f));
    EXPECT_EQ(-q2, XE::Quaternion<float>(0.0f, -1.0f, 0.0f, -1.0f));
}

TEST(QuaternionTest, PlusOperatorShouldModifyNoComponents) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(+q1, XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(+q2, XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f));
}

TEST(QuaternionTest, MultiplyByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 * 1.0f, XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(q2 * 0.0f, XE::Quaternion<float>(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(QuaternionTest, ScalarByQuaternionOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(1.0f * q1, XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(0.0f * q2, XE::Quaternion<float>(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(QuaternionTest, DivideByScalarOperatorMultiplyEachComponent) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_EQ(q1 / 1.0f, XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f));
    EXPECT_EQ(q2 / 0.5f, XE::Quaternion<float>(0.0f, 2.0f, 0.0f, 2.0f));
}


TEST(QuaternionTest, MultiplyByIdentityShouldNotHaveAnyAffect) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = XE::Quaternion<float>::createIdentity();

    EXPECT_EQ(q1 * qi, q1);
    EXPECT_EQ(q2 * qi, q2);
}

TEST(QuaternionTest, MultiplyByZeroShouldCollapseAnyQuaternionToZero) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = XE::Quaternion<float>::createZero();

    EXPECT_EQ(q1 * qz, qz);
    EXPECT_EQ(q2 * qz, qz);
}

TEST(QuaternionTest, DivideByIdentityAhouldNotAffectAnyQuaternion) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = XE::Quaternion<float>::createIdentity();

    EXPECT_EQ(q1 / qi, q1);
    EXPECT_EQ(q2 / qi, q2);
}

TEST(QuaternionTest, DivideByZeroShouldCollapseAnyQuaternionToNaNValues) {
    const auto q1 = XE::Quaternion<float>(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = XE::Quaternion<float>::createZero();

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
    XE::Quaternion<float> q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 += q1;

    EXPECT_FLOAT_EQ(q1.V.X, 2.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 2.0f);
    EXPECT_FLOAT_EQ(q1.W, 2.0f);
}

TEST(QuaternionTest, SubtractractAndAssignOperatorShouldSubtractEachComponentTogether) {
    XE::Quaternion<float> q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 -= XE::Quaternion<float>(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, -1.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 0.0f);
}

TEST(QuaternionTest, MultiplyAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quaternion<float> q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 *= 3.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 3.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q1.W, 3.0f);
}


TEST(QuaternionTest, DivideAndAssignByScalarOperatorMultiplyEachComponent) {
    XE::Quaternion<float> q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 /= 1.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 1.0f);
}


TEST(QuaternionTest, Norm2ShouldComputeSquaredMagnitude) {
    const auto q = XE::Quaternion<float>{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(XE::norm2(q), 50.0f);
}


TEST(QuaternionTest, NormShouldComputeMagnitude) {
    const auto q = XE::Quaternion<float>{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(XE::norm(q), 7.071067812f);
}


TEST(QuaternionTest, ConjugateShouldReturnNegatedVectorPart) {
    const auto q = XE::Quaternion<float>{3.0f, 4.0f, 5.0f, 1.0f};
    const auto conjugate = XE::conjugate(q);

    EXPECT_FLOAT_EQ(conjugate.V.X, -3.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Y, -4.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Z, -5.0f);
    EXPECT_FLOAT_EQ(conjugate.W, 1.0f);
}


TEST(QuaternionTest, InverseShouldComputeAnNormalizedCongujatedQuaternion) {
    const auto q1 = XE::Quaternion<float>{{3.0f, 4.0f, 5.0f}, 0.0f};
    const auto qinverse = XE::inverse(q1);

    EXPECT_FLOAT_EQ(qinverse.V.X, -3.0f / 50.0f);
    EXPECT_FLOAT_EQ(qinverse.V.Y, -4.0f / 50.0f);
    EXPECT_FLOAT_EQ(qinverse.V.Z, -5.0f / 50.0f);
    EXPECT_FLOAT_EQ(qinverse.W, 0.0f);
}
