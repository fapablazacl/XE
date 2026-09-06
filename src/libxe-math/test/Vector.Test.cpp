
#include <catch2/catch_all.hpp>

#include "xe/math/Vector.h"
#include <sstream>

TEST_CASE("VectorTest, DefaultConstructorShouldInitializeToZeroes") {
    const xe::Vector3 v;

    REQUIRE(v.x == 0.0f);
    REQUIRE(v.y == 0.0f);
    REQUIRE(v.z == 0.0f);
}

TEST_CASE("VectorTest, ConstructorShouldSetupTheVectorComponentsCorrectly") {
    const xe::Vector3 v = {1.0f, 2.0f, 3.0f};

    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);

    REQUIRE(v.values[0] == 1.0f);
    REQUIRE(v.values[1] == 2.0f);
    REQUIRE(v.values[2] == 3.0f);

    REQUIRE(v[0] == 1.0f);
    REQUIRE(v[1] == 2.0f);
    REQUIRE(v[2] == 3.0f);
}

TEST_CASE("VectorTest, ConstructorFromPointerShouldSetupTheVectorComponentsCorrectly") {
    const float data[] = {1.0f, 2.0f, 3.0f};
    const xe::Vector3 v{data};

    REQUIRE(v.x == 1.0f);
    REQUIRE(v.y == 2.0f);
    REQUIRE(v.z == 3.0f);
}

TEST_CASE("VectorTest, DataMethodShouldReturnAnAddressToTheFirstComponent") {
    xe::Vector3 varv;
    REQUIRE(varv.data() != nullptr);
    REQUIRE(varv.data() == &varv.values[0]);
}

TEST_CASE("VectorTest, ConstDataMethodShouldReturnAnAddressToTheFirstComponent") {
    const xe::Vector3 constv;
    REQUIRE(constv.data() != nullptr);
    REQUIRE(constv.data() == &constv.values[0]);
}

TEST_CASE("VectorTest, CastMethodShouldConvertUnderlyingType") {
    const xe::Vector3 vf{1.0f, 2.0f, 3.0f};
    const auto vd = vf.cast<double>();

    REQUIRE(vd.x == 1.0);
    REQUIRE(vd.y == 2.0);
    REQUIRE(vd.z == 3.0);
}

TEST_CASE("VectorTest, ComparisonOperatorsShouldCheckVectorComponentsForEqualityAndInequality") {
    const xe::Vector3 v1 = {1.0f, 2.0f, 3.0f};
    const xe::Vector3 v2 = {1.0f, 2.0f, 3.0f};
    const xe::Vector3 v3 = {-1.0f, -2.0f, -3.0f};

    REQUIRE(v1 == v2);
    REQUIRE(v1 != v3);
    REQUIRE(v2 != v3);

    REQUIRE(v2 == v1);
    REQUIRE(v3 != v1);
    REQUIRE(v3 != v2);
}

TEST_CASE("VectorTest, OperatorPlusShouldAddComponentWise") {
    const xe::Vector3 v1{1.0f, -2.0f, 3.0f};
    const xe::Vector3 v2{-2.0f, -1.0f, -2.0f};

    REQUIRE((v2 + v1) == xe::Vector3(-1.0f, -3.0f, 1.0f));
    REQUIRE((v1 + v2) == xe::Vector3(-1.0f, -3.0f, 1.0f));
    REQUIRE(v1 == +v1);
    REQUIRE(v2 == +v2);
    REQUIRE(((xe::Vector3(0.0f) += v1) == v1));
    REQUIRE(((xe::Vector3(0.0f) += v2) == v2));
}

TEST_CASE("VectorTest, OperatorMinusShouldSubtractComponentWise") {
    const xe::Vector3 v1{1.0f, -2.0f, 3.0f};
    const xe::Vector3 v2{-2.0f, -1.0f, -2.0f};

    REQUIRE((v2 - v1) == xe::Vector3(-3.0f, 1.0f, -5.0f));
    REQUIRE((v1 - v2) == xe::Vector3(3.0f, -1.0f, 5.0f));
    REQUIRE((v1 - v1) == xe::Vector3(0.0f));
    REQUIRE((v2 - v2) == xe::Vector3(0.0f));

    REQUIRE(xe::Vector3(-1.0f, 2.0f, -3.0f) == -v1);
    REQUIRE(xe::Vector3(2.0f, 1.0f, 2.0f) == -v2);

    REQUIRE(((xe::Vector3(0.0f) -= v1) == -v1));
    REQUIRE(((xe::Vector3(0.0f) -= v2) == -v2));
}

TEST_CASE("VectorTest, OperatorMulShouldMultiplyComponentWise") {
    const xe::Vector3 v1 = {1.0f, -2.0f, 3.0f};
    const xe::Vector3 v2 = {-2.0f, -1.0f, -2.0f};

    REQUIRE((v1 * v2) == xe::Vector3(-2.0f, 2.0f, -6.0f));
    REQUIRE((v2 * v1) == xe::Vector3(-2.0f, 2.0f, -6.0f));

    REQUIRE((v1 * 1.0f) == v1);
    REQUIRE((v1 * -1.0f) == -v1);
    REQUIRE((v1 * 2.0f) == xe::Vector3(2.0f, -4.0f, 6.0f));
    REQUIRE((v2 * 1.0f) == v2);
    REQUIRE((v2 * -1.0f) == -v2);
    REQUIRE((v2 * 2.0f) == xe::Vector3(-4.0f, -2.0f, -4.0f));

    REQUIRE((1.0f * v2) == v2);
    REQUIRE((-1.0f * v2) == -v2);
    REQUIRE((1.0f * v1) == v1);
    REQUIRE((-1.0f * v1) == -v1);

    REQUIRE((1.4f * v1) == (v1 * 1.4f));
    REQUIRE((1.4f * v2) == (v2 * 1.4f));

    REQUIRE(((xe::Vector3(1.0f) *= v1) == v1));
    REQUIRE(((xe::Vector3(1.0f) *= v2) == v2));
}

TEST_CASE("VectorTest, OperatorMulAndAssignByScalarShouldMultiplyComponentWise") {
    xe::Vector3 v1 = {1.0f, -2.0f, 3.0f};

    v1 *= -1.0f;

    REQUIRE(v1.x == -1.0f);
    REQUIRE(v1.y == 2.0f);
    REQUIRE(v1.z == -3.0f);
}

TEST_CASE("VectorTest, OperatorDivAndAssignByScalarShouldMultiplyComponentWise") {
    xe::Vector3 v1 = {2.0f, -2.0f, 4.0f};

    v1 /= 2.0f;

    REQUIRE(v1.x == 1.0f);
    REQUIRE(v1.y == -1.0f);
    REQUIRE(v1.z == 2.0f);
}

TEST_CASE("VectorTest, OperatorDivShouldDivideComponentWise") {
    const xe::Vector3 v1{2.0f, 8.0f, 32.0f};
    const xe::Vector3 v2{1.0f, 2.0f, 4.0f};

    REQUIRE((v1 / v2) == xe::Vector3(2.0f, 4.0f, 8.0f));
    REQUIRE((v1 / 2.0f) == xe::Vector3(1.0f, 4.0f, 16.0f));
    REQUIRE((v2 / 2.0f) == xe::Vector3(0.5f, 1.0f, 2.0f));

    REQUIRE(((xe::Vector3(v1) /= v1) == xe::Vector3(1.0f, 1.0f, 1.0f)));
    REQUIRE(((xe::Vector3(v2) /= v2) == xe::Vector3(1.0f, 1.0f, 1.0f)));
}

TEST_CASE("VectorTest, DotProductShouldComputeASumOfProductsOfEachComponent") {
    const xe::Vector3 v1 = {2.0f, 8.0f, 32.0f};
    const xe::Vector3 v2 = {1.0f, 2.0f, 4.0f};

    REQUIRE(dot(xe::Vector3(1.0f, 1.0f, 1.0f), v1) == Catch::Approx(42.0f));
    REQUIRE(dot(xe::Vector3(1.0f, 1.0f, 1.0f), v2) == Catch::Approx(7.0f));
    REQUIRE(dot(xe::Vector3(0.0f), v1) == Catch::Approx(0.0f));
    REQUIRE(dot(xe::Vector3(0.0f), v2) == Catch::Approx(0.0f));

    REQUIRE(dot(v1, xe::Vector3(1.0f, 1.0f, 1.0f)) == Catch::Approx(42.0f));
    REQUIRE(dot(v2, xe::Vector3(1.0f, 1.0f, 1.0f)) == Catch::Approx(7.0f));
    REQUIRE(dot(v1, xe::Vector3(0.0f)) == Catch::Approx(0.0f));
    REQUIRE(dot(v2, xe::Vector3(0.0f)) == Catch::Approx(0.0f));

    REQUIRE(dot(v1, v2) == Catch::Approx(146.0f));
    REQUIRE(dot(v2, v1) == Catch::Approx(146.0f));
}

TEST_CASE("VectorTest, TripleDotProductFromUnitAxisComputesOne") {
    const xe::Vector3 v1 = {1.0f, 0.0f, 0.0f};
    const xe::Vector3 v2 = {0.0f, 1.0f, 0.0f};
    const xe::Vector3 v3 = {0.0f, 0.0f, 1.0f};

    const float result = xe::dot(v1, v2, v3);

    REQUIRE(result == Catch::Approx(1.0f));
}

TEST_CASE("VectorTest, TripleDotProductFromCoplanarVectorComputesZero") {
    const xe::Vector3 v1 = {1.0f, 0.0f, 0.0f};
    const xe::Vector3 v2 = {0.0f, 1.0f, 0.0f};
    const xe::Vector3 v3 = {-1.0f, -1.0f, 0.0f};

    REQUIRE(xe::dot(v1, v2, v3) == Catch::Approx(0.0f));
}

TEST_CASE("VectorTest, TripleDotProductFromArbitraryAxesComputesSeven") {
    const xe::Vector3 v1 = {1.0f, -1.0f, 1.0f};
    const xe::Vector3 v2 = {2.0f, 1.0f, 1.0f};
    const xe::Vector3 v3 = {1.0f, 1.0f, -2.0f};

    const float result = xe::dot(v1, v2, v3);

    REQUIRE(result == Catch::Approx(-7.0f));
}

TEST_CASE("VectorTest, TripleDotProductDontChangeFromCircularRotationOfParameters") {
    const xe::Vector3 v1 = {1.0f, -1.0f, 1.0f};
    const xe::Vector3 v2 = {2.0f, 1.0f, 1.0f};
    const xe::Vector3 v3 = {1.0f, 1.0f, -2.0f};

    REQUIRE(xe::dot(v1, v2, v3) == Catch::Approx(xe::dot(v3, v1, v2)));
    REQUIRE(xe::dot(v1, v2, v3) == Catch::Approx(xe::dot(v2, v3, v1)));
}

TEST_CASE("VectorTest, TripleDotProductChangesSignFromParameterSwapping") {
    const xe::Vector3 v1 = {1.0f, -1.0f, 1.0f};
    const xe::Vector3 v2 = {2.0f, 1.0f, 1.0f};
    const xe::Vector3 v3 = {1.0f, 1.0f, -2.0f};

    REQUIRE(xe::dot(v1, v2, v3) == Catch::Approx(-xe::dot(v2, v1, v3)));
    REQUIRE(xe::dot(v1, v2, v3) == Catch::Approx(-xe::dot(v1, v3, v2)));
}

TEST_CASE("VectorTest, TwoDimensionalCrossProductShouldReturnCrossVectorLength") {
    const xe::Vector2 v1 = {2.0f, 0.0f};
    const xe::Vector2 v2 = {0.0f, 2.0f};
    const xe::Vector2 v3 = {0.0f, -1.0f};

    REQUIRE(cross(v1, v2) == 4.0f);
    REQUIRE(cross(v1, v3) == -2.0f);
}

TEST_CASE("VectorTest, ThreeDimensionalCrossProductShouldPerformCorrectlyForUnitVectors") {
    const xe::Vector3 v1 = {1.0f, 0.0f, 0.0f};
    const xe::Vector3 v2 = {0.0f, 1.0f, 0.0f};
    const xe::Vector3 v3 = {0.0f, 0.0f, 1.0f};

    REQUIRE(cross(v1, v2) == v3);
    REQUIRE(cross(v2, v1) == -v3);

    REQUIRE(cross(v2, v3) == v1);
    REQUIRE(cross(v3, v2) == -v1);

    REQUIRE(cross(v3, v1) == v2);
    REQUIRE(cross(v1, v3) == -v2);

    REQUIRE(cross(v1, v1) == xe::Vector3(0.0f));
    REQUIRE(cross(v2, v2) == xe::Vector3(0.0f));
    REQUIRE(cross(v3, v3) == xe::Vector3(0.0f));
}

TEST_CASE("VectorTest, TripleCrossProductComputesACB_Minus_ABC_Product") {
    const xe::Vector3 v1 = {1.0f, 0.0f, 0.0f};
    const xe::Vector3 v2 = {0.0f, 1.0f, 0.0f};
    const xe::Vector3 v3 = {0.0f, 0.0f, 1.0f};

    REQUIRE(cross(v1, v2, v3) == dot(v1, v3) * v2 - dot(v1, v2) * v3);
}

TEST_CASE("VectorTest, MinimizeShouldReturnMinimunValuesBetweenTwoVectors") {
    const xe::Vector3 v1 = {1.0f, -2.0f, -1.3f};
    const xe::Vector3 v2 = {1.3f, -1.6f, 0.0f};
    const xe::Vector3 v3 = {-0.3f, 2.0f, 2.0f};

    REQUIRE(minimize(v1, v1) == v1);
    REQUIRE(minimize(v2, v2) == v2);
    REQUIRE(minimize(v3, v3) == v3);

    REQUIRE(minimize(v1, v2) == xe::Vector3(1.0f, -2.0f, -1.3f));
    REQUIRE(minimize(v1, v3) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v2, v3) == xe::Vector3(-0.3f, -1.6f, 0.0f));

    REQUIRE(minimize(v2, v1) == xe::Vector3(1.0f, -2.0f, -1.3f));
    REQUIRE(minimize(v3, v1) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v3, v2) == xe::Vector3(-0.3f, -1.6f, 0.0f));

    REQUIRE(minimize(v1, minimize(v2, v3)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v1, minimize(v3, v2)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v2, minimize(v1, v3)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v2, minimize(v3, v1)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v3, minimize(v2, v1)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
    REQUIRE(minimize(v3, minimize(v1, v2)) == xe::Vector3(-0.3f, -2.0f, -1.3f));
}

TEST_CASE("VectorTest, MaximizeFunctionShouldReturnTheMaximunValuesBetweenTwoVectors") {
    const xe::Vector3 v1 = {1.0f, -2.0f, -1.3f};
    const xe::Vector3 v2 = {1.3f, -1.6f, 0.0f};
    const xe::Vector3 v3 = {-0.3f, 2.0f, 2.0f};

    REQUIRE(maximize(v1, v1) == v1);
    REQUIRE(maximize(v2, v2) == v2);
    REQUIRE(maximize(v3, v3) == v3);

    REQUIRE(maximize(v1, v2) == xe::Vector3(1.3f, -1.6f, 0.0f));
    REQUIRE(maximize(v1, v3) == xe::Vector3(1.0f, 2.0f, 2.0f));
    REQUIRE(maximize(v2, v3) == xe::Vector3(1.3f, 2.0f, 2.0f));

    REQUIRE(maximize(v2, v1) == xe::Vector3(1.3f, -1.6f, 0.0f));
    REQUIRE(maximize(v3, v1) == xe::Vector3(1.0f, 2.0f, 2.0f));
    REQUIRE(maximize(v3, v2) == xe::Vector3(1.3f, 2.0f, 2.0f));

    REQUIRE(maximize(v1, maximize(v2, v3)) == xe::Vector3(1.3f, 2.0f, 2.0f));
    REQUIRE(maximize(v1, maximize(v3, v2)) == xe::Vector3(1.3f, 2.0f, 2.0f));
    REQUIRE(maximize(v2, maximize(v1, v3)) == xe::Vector3(1.3f, 2.0f, 2.0f));
    REQUIRE(maximize(v2, maximize(v3, v1)) == xe::Vector3(1.3f, 2.0f, 2.0f));
    REQUIRE(maximize(v3, maximize(v2, v1)) == xe::Vector3(1.3f, 2.0f, 2.0f));
    REQUIRE(maximize(v3, maximize(v1, v2)) == xe::Vector3(1.3f, 2.0f, 2.0f));
}

TEST_CASE("VectorTest, Norm2FunctionShouldReturnAnSquaredVectorLength") {
    const xe::Vector3 v1 = {1.0f, -2.0f, -1.6f};
    const xe::Vector3 v2 = {1.3f, -1.6f, 0.0f};
    const xe::Vector3 v3 = {-0.3f, 2.0f, 2.0f};

    REQUIRE(norm2(v1) == Catch::Approx(1.0f + 4.0f + 1.6f * 1.6f));
    REQUIRE(norm2(v2) == Catch::Approx(1.3f * 1.3f + 1.6f * 1.6f));
    REQUIRE(norm2(v3) == Catch::Approx(0.3f * 0.3f + 4.0f + 4.0f));
}

TEST_CASE("VectorTest, NormFunctionShouldReturnTheVectorLength") {
    const xe::Vector3 v1 = {0.0f, 0.0f, 0.0f};
    const xe::Vector3 v2 = {4.0f, 2.0f, 4.0f};
    const xe::Vector3 v3 = {1.0f, 1.0f, 1.0f};

    REQUIRE(norm(v1) == Catch::Approx(0.0f));
    REQUIRE(norm(v2) == Catch::Approx(6.0f));
    REQUIRE(norm(v3) == Catch::Approx(std::sqrt(3.0f)));
}
