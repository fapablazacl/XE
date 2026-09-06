
#include "xe/math/Quaternion.h"
#include <catch2/catch_test_macros.hpp>
#include "xe/math/Common.h"

TEST_CASE("QuaternionTest, VectorScalarConstructorInitializesTheVectorAndScalarPart") {
    const auto q2 = xe::Quat{{0.0f, 1.0f, 0.0f}, 1.0f};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q2.W, 1.0f);
}

TEST_CASE("QuaternionTest, FourScalarConstructorInitializesTheVectorAndScalarPart") {
    const auto q = xe::Quat{0.0f, 1.0f, 0.0f, 1.0f};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST_CASE("QuaternionTest, OneScalarConstructorInitializesTheScalarPart") {
    const auto q2 = xe::Quat{1.0f};

    EXPECT_FLOAT_EQ(q2.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q2.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q2.W, 1.0f);
}

TEST_CASE("QuaternionTest, Vector3ConstructorConstructorInitializesTheVectorPart") {
    const auto q = xe::Quat{{0.0f, 1.0f, 0.0f}};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST_CASE("QuaternionTest, Vector4ConstructorConstructorInitializesTheVectorPart") {
    const auto v = xe::Vector4{0.0f, 1.0f, 0.0f, 10.0f};
    const auto q = xe::Quat{v};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 10.0f);
}

TEST_CASE("QuaternionTest, ThreeScalarConstructorInitializesTheVectorPart") {
    const auto q = xe::Quat{0.0f, 1.0f, 0.0f};

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST_CASE("QuaternionTest, PointerConstructorInitializesTheVectorAndScalarPart") {
    const float values[] = {4.0f, 2.0f, 3.0f, 1.0f};

    const auto q = xe::Quat{values};

    EXPECT_FLOAT_EQ(q.V.X, 4.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST_CASE("QuaternionTest, CopyConstructorInitializesTheVectorAndScalarPart") {
    const auto q = xe::Quat{xe::Quat{1.0f, 2.0f, 3.0f, 1.0f}};

    EXPECT_FLOAT_EQ(q.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 2.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST_CASE("QuaternionTest, DataShouldReturnAPointerToTheFirstElement") {
    const auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    const float *data = quat.data();

    REQUIRE(data[0], 0.0f);
    REQUIRE(data[1], 1.0f);
    REQUIRE(data[2], 2.0f);
    REQUIRE(data[3], 3.0f);
}

TEST_CASE("QuaternionTest, SizeAlwaysReturnsFour") {
    const auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    REQUIRE(quat.size(), 4);
}

TEST_CASE("QuaternionTest, OperatorBracketsShouldEnableAccessToEachElement") {
    auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    REQUIRE(quat[0], 0.0f);
    REQUIRE(quat[1], 1.0f);
    REQUIRE(quat[2], 2.0f);
    REQUIRE(quat[3], 3.0f);

    quat[3] = 10.0f;
    REQUIRE(quat[3], 10.0f);

    const auto quat2 = xe::Quat{10.0f, -1.0f, -2.0f, -3.0f};
    REQUIRE(quat2[0], 10.0f);
    REQUIRE(quat2[1], -1.0f);
    REQUIRE(quat2[2], -2.0f);
    REQUIRE(quat2[3], -3.0f);
}

TEST_CASE("QuaternionTest, EqualityOperatorDoesElementWiseComparisonViaFPTolerance") {
    const auto qa_1 = xe::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = xe::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = xe::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = xe::Quat{1.0f, 2.0f, 3.0f};

    REQUIRE(qa_1, qa_2);
    REQUIRE(qb_1, qb_2);
    REQUIRE(qa_2, qa_1);
    REQUIRE(qb_2, qb_1);
}

TEST_CASE("QuaternionTest, InequalityOperatorDoesElementWiseComparisonViaFPTolerance") {
    const auto qa_1 = xe::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = xe::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = xe::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = xe::Quat{1.0f, 2.0f, 3.0f};

    EXPECT_NE(qa_1, qb_2);
    EXPECT_NE(qa_2, qb_1);

    EXPECT_NE(qb_1, qa_2);
    EXPECT_NE(qb_2, qa_1);
}

TEST_CASE("QuaternionTest, ZeroQuaternionFactoryMethodInitializesScalarAndVectorPartToZeroes") {
    const auto q = xe::quatZero<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 0.0f);
}

TEST_CASE("QuaternionTest, IdentityQuaternionFactoryMethodInitializesScalarPartToOne") {
    const auto q = xe::quatId<float>();

    EXPECT_FLOAT_EQ(q.V.X, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q.V.Z, 0.0f);
    EXPECT_FLOAT_EQ(q.W, 1.0f);
}

TEST_CASE("QuaternionTest, RotationRHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine") {
    const float radians = xe::pi<float>;
    const xe::Vector3 axis{0.0f, 1.0f, 0.0f};

    const auto subject = xe::quatRotationRH<float>(axis, radians);
    const auto correct = xe::normalize(xe::Quat{axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    EXPECT_FLOAT_EQ(xe::norm(subject), 1.0f);
    EXPECT_FLOAT_EQ(subject.V.X, correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST_CASE("QuaternionTest, RotationLHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine") {
    const float radians = xe::pi<float>;
    const xe::Vector3 axis{0.0f, 1.0f, 0.0f};

    const auto subject = xe::quatRotationLH<float>(axis, radians);
    const auto correct = xe::normalize(xe::Quat{axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    EXPECT_FLOAT_EQ(subject.V.X, -correct.V.X);
    EXPECT_FLOAT_EQ(subject.V.Y, -correct.V.Y);
    EXPECT_FLOAT_EQ(subject.V.Z, -correct.V.Z);
    EXPECT_FLOAT_EQ(subject.W, correct.W);
}

TEST_CASE("QuaternionTest, DotShouldComputeThwSumOfProductsElementWise") {
    const xe::Quat v1 = {2.0f, 8.0f, 32.0f, 0.0f};
    const xe::Quat v2 = {1.0f, 2.0f, 4.0f, 0.0f};

    EXPECT_FLOAT_EQ(dot(v1, v2), 146.0f);
    EXPECT_FLOAT_EQ(dot(v2, v1), 146.0f);
}

TEST_CASE("QuaternionTest, Norm2ShouldComputeTheMagnitudeSquared") {
    const auto q = xe::Quat{2.0f, 3.0f, 4.0f, 5.0f};
    REQUIRE(norm2(q), 54.0f);
}

TEST_CASE("QuaternionTest, NormShouldComputeTheMagnitude") {
    const auto q = xe::Quat{0.0f, 3.0f, 0.0f, 4.0f};
    REQUIRE(norm(q), 5.0f);
}

TEST_CASE("QuaternionTest, ConjugateShouldNegateTheVectorPart") {
    const auto q4 = xe::conjugate(xe::Quat{{-1.0f, 1.0f, -1.0f}, 1.0f});

    REQUIRE(q4.V.X, 1.0f);
    REQUIRE(q4.V.Y, -1.0f);
    REQUIRE(q4.V.Z, 1.0f);
    REQUIRE(q4.W, 1.0f);
}

TEST_CASE("QuaternionTest, NormalizeShouldScaleAQuaternionToTheUnitLength") {
    const auto q1 = xe::normalize(xe::Quat{{1.0f, 1.0f, 1.0f}, 1.0f});
    REQUIRE(q1.V.X, 0.5f);
    REQUIRE(q1.V.Y, 0.5f);
    REQUIRE(q1.V.Z, 0.5f);
    REQUIRE(q1.W, 0.5f);

    const auto q2 = xe::normalize(xe::Quat{{0.0f, 4.0f, 0.0f}, 0.0f});
    REQUIRE(q2.V.X, 0.0f);
    REQUIRE(q2.V.Y, 1.0f);
    REQUIRE(q2.V.Z, 0.0f);
    REQUIRE(q2.W, 0.0f);

    const auto q3 = xe::normalize(xe::Quat{{0.0f, -1.0f, 0.0f}, 0.0f});
    REQUIRE(q3.V.X, 0.0f);
    REQUIRE(q3.V.Y, -1.0f);
    REQUIRE(q3.V.Z, 0.0f);
    REQUIRE(q3.W, 0.0f);
}

TEST_CASE("QuaternionTest, AdditionOperatorShouldAddTheirEachComponentsTogether") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(q1 + q1 == xe::Quat(2.0f, 0.0f, 2.0f, 2.0f));
    REQUIRE(q1 + q2 == xe::Quat(1.0f, 1.0f, 1.0f, 2.0f));
}

TEST_CASE("QuaternionTest, SubtractionOperatorShouldSubtractEachComponentTogether") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(q1 - q1 == xe::Quat(0.0f, 0.0f, 0.0f, 0.0f));
    REQUIRE(q1 - q2 == xe::Quat(1.0f, -1.0f, 1.0f, 0.0f));
}

TEST_CASE("QuaternionTest, NegationOperatorShouldInvertEachComponent") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(-q1 == xe::Quat(-1.0f, -0.0f, -1.0f, -1.0f));
    REQUIRE(-q2 == xe::Quat(0.0f, -1.0f, 0.0f, -1.0f));
}

TEST_CASE("QuaternionTest, PlusOperatorShouldModifyNoComponents") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(+q1 == xe::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    REQUIRE(+q2 == xe::Quat(0.0f, 1.0f, 0.0f, 1.0f));
}

TEST_CASE("QuaternionTest, MultiplyByScalarOperatorMultiplyEachComponent") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(q1 * 1.0f == xe::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    REQUIRE(q2 * 0.0f == xe::Quat(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST_CASE("QuaternionTest, ScalarByQuaternionOperatorMultiplyEachComponent") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(1.0f * q1 == xe::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    REQUIRE(0.0f * q2 == xe::Quat(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST_CASE("QuaternionTest, DivideByScalarOperatorMultiplyEachComponent") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(q1 / 1.0f == xe::Quat(1.0f, 0.0f, 1.0f, 1.0f));
    REQUIRE(q2 / 0.5f == xe::Quat(0.0f, 2.0f, 0.0f, 2.0f));
}

TEST_CASE("QuaternionTest, MultiplyShouldCombineTwoQuaternions") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 0.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

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

TEST_CASE("QuaternionTest, MultiplyByIdentityQShouldNotHaveAnyAffect") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = xe::quatId<float>();

    REQUIRE(q1 * qi, q1);
    REQUIRE(q2 * qi, q2);
}

TEST_CASE("QuaternionTest, MultiplyByZeroQShouldCollapseAnyQuaternionToZero") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = xe::quatZero<float>();

    REQUIRE(q1 * qz, qz);
    REQUIRE(q2 * qz, qz);
}

TEST_CASE("QuaternionTest, DivideByIdentityShouldNotAffectAnyQuaternion") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = xe::quatId<float>();

    REQUIRE(q1 / qi, q1);
    REQUIRE(q2 / qi, q2);
}

TEST_CASE("QuaternionTest, DivideByZeroShouldCollapseAnyQuaternionToNaNValues") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = xe::quatZero<float>();

    const auto r1 = q1 / qz;
    const auto r2 = q2 / qz;

    REQUIRE(std::isnan(r1.V.X));
    REQUIRE(std::isnan(r1.V.Y));
    REQUIRE(std::isnan(r1.V.Z));
    REQUIRE(std::isnan(r1.W));

    REQUIRE(std::isnan(r2.V.X));
    REQUIRE(std::isnan(r2.V.Y));
    REQUIRE(std::isnan(r2.V.Z));
    REQUIRE(std::isnan(r2.W));
}

TEST_CASE("QuaternionTest, AddAndAssignOperatorShouldAddTheirEachComponentsTogether") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 += q1;

    EXPECT_FLOAT_EQ(q1.V.X, 2.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 2.0f);
    EXPECT_FLOAT_EQ(q1.W, 2.0f);
}

TEST_CASE("QuaternionTest, SubtractractAndAssignOperatorShouldSubtractEachComponentTogether") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 -= xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, -1.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 0.0f);
}

TEST_CASE("QuaternionTest, MultiplyAndAssignByScalarOperatorMultiplyEachComponent") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 *= 3.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 3.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 3.0f);
    EXPECT_FLOAT_EQ(q1.W, 3.0f);
}

TEST_CASE("QuaternionTest, DivideAndAssignByScalarOperatorMultiplyEachComponent") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 /= 1.0f;

    EXPECT_FLOAT_EQ(q1.V.X, 1.0f);
    EXPECT_FLOAT_EQ(q1.V.Y, 0.0f);
    EXPECT_FLOAT_EQ(q1.V.Z, 1.0f);
    EXPECT_FLOAT_EQ(q1.W, 1.0f);
}

TEST_CASE("QuaternionTest, Norm2ShouldComputeSquaredMagnitude") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(xe::norm2(q), 50.0f);
}

TEST_CASE("QuaternionTest, NormShouldComputeMagnitude") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    EXPECT_FLOAT_EQ(xe::norm(q), 7.071067812f);
}

TEST_CASE("QuaternionTest, ConjugateShouldReturnNegatedVectorPart") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 1.0f};
    const auto conjugate = xe::conjugate(q);

    EXPECT_FLOAT_EQ(conjugate.V.X, -3.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Y, -4.0f);
    EXPECT_FLOAT_EQ(conjugate.V.Z, -5.0f);
    EXPECT_FLOAT_EQ(conjugate.W, 1.0f);
}

TEST_CASE("QuaternionTest, NormalizeShouldReturnUnitLengthQuaternion") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};
    const auto normalized = xe::normalize(q);

    EXPECT_FLOAT_EQ(xe::norm(normalized), 1.0f);
}

TEST_CASE("QuaternionTest, InverseShouldComputeAnNormalizedCongujatedQuaternion") {
    const auto q1 = xe::Quat{{3.0f, 4.0f, 5.0f}, 0.0f};
    const auto inverse = xe::inverse(q1);

    EXPECT_FLOAT_EQ(inverse.V.X, -3.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.V.Y, -4.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.V.Z, -5.0f / 50.0f);
    EXPECT_FLOAT_EQ(inverse.W, 0.0f);
}

TEST_CASE("QuaternionTest, TransformShouldRotatePointVector") {
    const auto rotation = xe::quatRotationRH({0.0f, 1.0f, 0.0f} == xe::pi<float> * 0.5f);
    const auto point = xe::Vector3{1.0f, 0.0f, 0.0f};
    const auto result = xe::transform(rotation, point);
    const auto e = xe::DEFAULT_EPSILON<float>;

    EXPECT_NEAR(result.X, 0.0f, e);
    EXPECT_NEAR(result.Y, 0.0f, e);
    EXPECT_NEAR(result.Z, -1.0f, e);
}
