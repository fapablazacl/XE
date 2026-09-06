#include <catch2/catch_all.hpp>
#include <cmath>
#include "xe/math/Quaternion.h"
#include "xe/math/Common.h"

TEST_CASE("QuaternionTest, VectorScalarConstructorInitializesTheVectorAndScalarPart") {
    const auto q2 = xe::Quat{{0.0f, 1.0f, 0.0f}, 1.0f};

    REQUIRE(q2.V.x == Catch::Approx(0.0f));
    REQUIRE(q2.V.y == Catch::Approx(1.0f));
    REQUIRE(q2.V.z == Catch::Approx(0.0f));
    REQUIRE(q2.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, FourScalarConstructorInitializesTheVectorAndScalarPart") {
    const auto q = xe::Quat{0.0f, 1.0f, 0.0f, 1.0f};

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(1.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, OneScalarConstructorInitializesTheScalarPart") {
    const auto q2 = xe::Quat{1.0f};

    REQUIRE(q2.V.x == Catch::Approx(0.0f));
    REQUIRE(q2.V.y == Catch::Approx(0.0f));
    REQUIRE(q2.V.z == Catch::Approx(0.0f));
    REQUIRE(q2.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, Vector3ConstructorConstructorInitializesTheVectorPart") {
    const auto q = xe::Quat{{0.0f, 1.0f, 0.0f}};

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(1.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(0.0f));
}

TEST_CASE("QuaternionTest, Vector4ConstructorConstructorInitializesTheVectorPart") {
    const auto v = xe::Vector4{0.0f, 1.0f, 0.0f, 10.0f};
    const auto q = xe::Quat{v};

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(1.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(10.0f));
}

TEST_CASE("QuaternionTest, ThreeScalarConstructorInitializesTheVectorPart") {
    const auto q = xe::Quat{0.0f, 1.0f, 0.0f};

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(1.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(0.0f));
}

TEST_CASE("QuaternionTest, PointerConstructorInitializesTheVectorAndScalarPart") {
    const float values[] = {4.0f, 2.0f, 3.0f, 1.0f};

    const auto q = xe::Quat{values};

    REQUIRE(q.V.x == Catch::Approx(4.0f));
    REQUIRE(q.V.y == Catch::Approx(2.0f));
    REQUIRE(q.V.z == Catch::Approx(3.0f));
    REQUIRE(q.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, CopyConstructorInitializesTheVectorAndScalarPart") {
    const auto q = xe::Quat{xe::Quat{1.0f, 2.0f, 3.0f, 1.0f}};

    REQUIRE(q.V.x == Catch::Approx(1.0f));
    REQUIRE(q.V.y == Catch::Approx(2.0f));
    REQUIRE(q.V.z == Catch::Approx(3.0f));
    REQUIRE(q.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, DataShouldReturnAPointerToTheFirstElement") {
    const auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    const float *data = quat.data();

    REQUIRE(data[0] == 0.0f);
    REQUIRE(data[1] == 1.0f);
    REQUIRE(data[2] == 2.0f);
    REQUIRE(data[3] == 3.0f);
}

TEST_CASE("QuaternionTest, SizeAlwaysReturnsFour") {
    const auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    REQUIRE(quat.size() == 4);
}

TEST_CASE("QuaternionTest, OperatorBracketsShouldEnableAccessToEachElement") {
    auto quat = xe::Quat{0.0f, 1.0f, 2.0f, 3.0f};
    REQUIRE(quat[0] == 0.0f);
    REQUIRE(quat[1] == 1.0f);
    REQUIRE(quat[2] == 2.0f);
    REQUIRE(quat[3] == 3.0f);

    quat[3] = 10.0f;
    REQUIRE(quat[3] == 10.0f);

    const auto quat2 = xe::Quat{10.0f, -1.0f, -2.0f, -3.0f};
    REQUIRE(quat2[0] == 10.0f);
    REQUIRE(quat2[1] == -1.0f);
    REQUIRE(quat2[2] == -2.0f);
    REQUIRE(quat2[3] == -3.0f);
}

TEST_CASE("QuaternionTest, EqualityOperatorDoesElementWiseComparisonViaFPTolerance") {
    const auto qa_1 = xe::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = xe::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = xe::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = xe::Quat{1.0f, 2.0f, 3.0f};

    REQUIRE(qa_1 == qa_2);
    REQUIRE(qb_1 == qb_2);
    REQUIRE(qa_2 == qa_1);
    REQUIRE(qb_2 == qb_1);
}

TEST_CASE("QuaternionTest, InequalityOperatorDoesElementWiseComparisonViaFPTolerance") {
    const auto qa_1 = xe::Quat{{1.0f, 2.0f, 3.0f}, 1.0f};
    const auto qa_2 = xe::Quat{1.0f, 2.0f, 3.0f, 1.0f};
    const auto qb_1 = xe::Quat{{1.0f, 2.0f, 3.0f}};
    const auto qb_2 = xe::Quat{1.0f, 2.0f, 3.0f};

    REQUIRE(qa_1 != qb_2);
    REQUIRE(qa_2 != qb_1);

    REQUIRE(qb_1 != qa_2);
    REQUIRE(qb_2 != qa_1);
}

TEST_CASE("QuaternionTest, ZeroQuaternionFactoryMethodInitializesScalarAndVectorPartToZeroes") {
    const auto q = xe::quatZero<float>();

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(0.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(0.0f));
}

TEST_CASE("QuaternionTest, IdentityQuaternionFactoryMethodInitializesScalarPartToOne") {
    const auto q = xe::quatId<float>();

    REQUIRE(q.V.x == Catch::Approx(0.0f));
    REQUIRE(q.V.y == Catch::Approx(0.0f));
    REQUIRE(q.V.z == Catch::Approx(0.0f));
    REQUIRE(q.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, RotationRHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine") {
    const float radians = xe::pi<float>;
    const xe::Vector3 axis{0.0f, 1.0f, 0.0f};

    const auto subject = xe::quatRotationRH<float>(axis, radians);
    const auto correct = xe::normalize(xe::Quat{axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    REQUIRE(xe::norm(subject) == Catch::Approx(1.0f));
    REQUIRE(subject.V.x == Catch::Approx(correct.V.x));
    REQUIRE(subject.V.y == Catch::Approx(correct.V.y));
    REQUIRE(subject.V.z == Catch::Approx(correct.V.z));
    REQUIRE(subject.W == Catch::Approx(correct.W));
}

TEST_CASE("QuaternionTest, RotationLHQuaternionFactoryMethodInitializesNormalizedQuaternionWithHalfAngleCosine") {
    const float radians = xe::pi<float>;
    const xe::Vector3 axis{0.0f, 1.0f, 0.0f};

    const auto subject = xe::quatRotationLH<float>(axis, radians);
    const auto correct = xe::normalize(xe::Quat{axis * std::sin(radians * 0.5f), std::cos(radians * 0.5f)});

    REQUIRE(subject.V.x == Catch::Approx(-correct.V.x));
    REQUIRE(subject.V.y == Catch::Approx(-correct.V.y));
    REQUIRE(subject.V.z == Catch::Approx(-correct.V.z));
    REQUIRE(subject.W == Catch::Approx(correct.W));
}

TEST_CASE("QuaternionTest, Norm2ShouldComputeTheSquaredMagnitude") {
    const auto q = xe::Quat{2.0f, 3.0f, 4.0f, 5.0f};
    REQUIRE(norm2(q) == 54.0f);
}

TEST_CASE("QuaternionTest, NormShouldComputeTheMagnitude") {
    const auto q = xe::Quat{0.0f, 3.0f, 0.0f, 4.0f};
    REQUIRE(norm(q) == 5.0f);
}

TEST_CASE("QuaternionTest, ConjugateShouldNegateTheVectorPart") {
    const auto q4 = xe::conjugate(xe::Quat{{-1.0f, 1.0f, -1.0f}, 1.0f});

    REQUIRE(q4.V.x == 1.0f);
    REQUIRE(q4.V.y == -1.0f);
    REQUIRE(q4.V.z == 1.0f);
    REQUIRE(q4.W == 1.0f);
}

TEST_CASE("QuaternionTest, NormalizeShouldScaleAQuaternionToTheUnitLength") {
    const auto q1 = xe::normalize(xe::Quat{{1.0f, 1.0f, 1.0f}, 1.0f});
    REQUIRE(q1.V.x == 0.5f);
    REQUIRE(q1.V.y == 0.5f);
    REQUIRE(q1.V.z == 0.5f);
    REQUIRE(q1.W == 0.5f);

    const auto q2 = xe::normalize(xe::Quat{{0.0f, 4.0f, 0.0f}, 0.0f});
    REQUIRE(q2.V.x == 0.0f);
    REQUIRE(q2.V.y == 1.0f);
    REQUIRE(q2.V.z == 0.0f);
    REQUIRE(q2.W == 0.0f);

    const auto q3 = xe::normalize(xe::Quat{{0.0f, -1.0f, 0.0f}, 0.0f});
    REQUIRE(q3.V.x == 0.0f);
    REQUIRE(q3.V.y == -1.0f);
    REQUIRE(q3.V.z == 0.0f);
    REQUIRE(q3.W == 0.0f);
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
    REQUIRE(r1.V.x == Catch::Approx(1.0f));
    REQUIRE(r1.V.y == Catch::Approx(1.0f));
    REQUIRE(r1.V.z == Catch::Approx(1.0f));
    REQUIRE(r1.W == Catch::Approx(1.0f));

    const auto r2 = q2 * q1;
    REQUIRE(r2.V.x == Catch::Approx(1.0f));
    REQUIRE(r2.V.y == Catch::Approx(1.0f));
    REQUIRE(r2.V.z == Catch::Approx(-1.0f));
    REQUIRE(r2.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, MultiplyByIdentityQShouldNotHaveAnyAffect") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = xe::quatId<float>();

    REQUIRE(q1 * qi == q1);
    REQUIRE(q2 * qi == q2);
}

TEST_CASE("QuaternionTest, MultiplyByZeroQShouldCollapseAnyQuaternionToZero") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = xe::quatZero<float>();

    REQUIRE(q1 * qz == qz);
    REQUIRE(q2 * qz == qz);
}

TEST_CASE("QuaternionTest, DivideByIdentityShouldNotAffectAnyQuaternion") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qi = xe::quatId<float>();

    REQUIRE(q1 / qi == q1);
    REQUIRE(q2 / qi == q2);
}

TEST_CASE("QuaternionTest, DivideByZeroShouldCollapseAnyQuaternionToNaNValues") {
    const auto q1 = xe::Quat(1.0f, 0.0f, 1.0f, 1.0f);
    const auto q2 = xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);
    const auto qz = xe::quatZero<float>();

    const auto r1 = q1 / qz;
    const auto r2 = q2 / qz;

    REQUIRE(std::isnan(r1.V.x));
    REQUIRE(std::isnan(r1.V.y));
    REQUIRE(std::isnan(r1.V.z));
    REQUIRE(std::isnan(r1.W));

    REQUIRE(std::isnan(r2.V.x));
    REQUIRE(std::isnan(r2.V.y));
    REQUIRE(std::isnan(r2.V.z));
    REQUIRE(std::isnan(r2.W));
}

TEST_CASE("QuaternionTest, AddAndAssignOperatorShouldAddTheirEachComponentsTogether") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 += q1;

    REQUIRE(q1.V.x == Catch::Approx(2.0f));
    REQUIRE(q1.V.y == Catch::Approx(0.0f));
    REQUIRE(q1.V.z == Catch::Approx(2.0f));
    REQUIRE(q1.W == Catch::Approx(2.0f));
}

TEST_CASE("QuaternionTest, SubtractractAndAssignOperatorShouldSubtractEachComponentTogether") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};
    q1 -= xe::Quat(0.0f, 1.0f, 0.0f, 1.0f);

    REQUIRE(q1.V.x == Catch::Approx(1.0f));
    REQUIRE(q1.V.y == Catch::Approx(-1.0f));
    REQUIRE(q1.V.z == Catch::Approx(1.0f));
    REQUIRE(q1.W == Catch::Approx(0.0f));
}

TEST_CASE("QuaternionTest, MultiplyAndAssignByScalarOperatorMultiplyEachComponent") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 *= 3.0f;

    REQUIRE(q1.V.x == Catch::Approx(3.0f));
    REQUIRE(q1.V.y == Catch::Approx(0.0f));
    REQUIRE(q1.V.z == Catch::Approx(3.0f));
    REQUIRE(q1.W == Catch::Approx(3.0f));
}

TEST_CASE("QuaternionTest, DivideAndAssignByScalarOperatorMultiplyEachComponent") {
    xe::Quat q1{{1.0f, 0.0f, 1.0f}, 1.0f};

    q1 /= 1.0f;

    REQUIRE(q1.V.x == Catch::Approx(1.0f));
    REQUIRE(q1.V.y == Catch::Approx(0.0f));
    REQUIRE(q1.V.z == Catch::Approx(1.0f));
    REQUIRE(q1.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, Norm2ShouldComputeSquaredMagnitude") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    REQUIRE(xe::norm2(q) == Catch::Approx(50.0f));
}

TEST_CASE("QuaternionTest, NormShouldComputeMagnitude") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};

    REQUIRE(xe::norm(q) == Catch::Approx(7.071067812f));
}

TEST_CASE("QuaternionTest, ConjugateShouldReturnNegatedVectorPart") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 1.0f};
    const auto conjugate = xe::conjugate(q);

    REQUIRE(conjugate.V.x == Catch::Approx(-3.0f));
    REQUIRE(conjugate.V.y == Catch::Approx(-4.0f));
    REQUIRE(conjugate.V.z == Catch::Approx(-5.0f));
    REQUIRE(conjugate.W == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, NormalizeShouldReturnUnitLengthQuaternion") {
    const auto q = xe::Quat{3.0f, 4.0f, 5.0f, 0.0f};
    const auto normalized = xe::normalize(q);

    REQUIRE(xe::norm(normalized) == Catch::Approx(1.0f));
}

TEST_CASE("QuaternionTest, InverseShouldComputeAnNormalizedCongujatedQuaternion") {
    const auto q1 = xe::Quat{{3.0f, 4.0f, 5.0f}, 0.0f};
    const auto inverse = xe::inverse(q1);

    REQUIRE(inverse.V.x == Catch::Approx(-3.0f / 50.0f));
    REQUIRE(inverse.V.y == Catch::Approx(-4.0f / 50.0f));
    REQUIRE(inverse.V.z == Catch::Approx(-5.0f / 50.0f));
    REQUIRE(inverse.W == Catch::Approx(0.0f));
}

TEST_CASE("QuaternionTest, TransformShouldRotatePointVector") {
    const auto rotation = xe::quatRotationRH({0.0f, 1.0f, 0.0f}, xe::pi<float> * 0.5f);
    const auto point = xe::Vector3{1.0f, 0.0f, 0.0f};
    const auto result = xe::transform(rotation, point);
    const auto e = xe::DEFAULT_EPSILON<float>;

    REQUIRE(result.x == Catch::Approx(0.0f).margin(e));
    REQUIRE(result.y == Catch::Approx(0.0f).margin(e));
    REQUIRE(result.z == Catch::Approx(-1.0f).margin(e));
}
