

#include "xe/math/Common.h"
#include "xe/math/Matrix.h"
#include "xe/math/Vector.h"
#include <cmath>
#include <gtest/gtest.h>

TEST(MatrixTest, DefaultConstructorShouldInitializeToZeroes) {
    XE::Matrix4 mat;

    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(mat.data()[i], 0.0F);
    }
}

TEST(MatrixTest, PointerConstructorShouldInterpretMatrixAsRowMajor) {
    float values[] = {1.0F, 2.0F, 3.0F, 4.0, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F, 13.0F, 14.0F, 15.0F, 16.0F};

    XE::Matrix4 mat{values};

    EXPECT_EQ(mat.data()[0], 1.0F);
    EXPECT_EQ(mat.data()[1], 2.0F);
    EXPECT_EQ(mat.data()[2], 3.0F);
    EXPECT_EQ(mat.data()[3], 4.0F);
    EXPECT_EQ(mat.data()[4], 5.0F);
    EXPECT_EQ(mat.data()[5], 6.0F);
    EXPECT_EQ(mat.data()[6], 7.0F);
    EXPECT_EQ(mat.data()[7], 8.0F);
    EXPECT_EQ(mat.data()[8], 9.0F);
    EXPECT_EQ(mat.data()[9], 10.0F);
    EXPECT_EQ(mat.data()[10], 11.0F);
    EXPECT_EQ(mat.data()[11], 12.0F);
    EXPECT_EQ(mat.data()[12], 13.0F);
    EXPECT_EQ(mat.data()[13], 14.0F);
    EXPECT_EQ(mat.data()[14], 15.0F);
    EXPECT_EQ(mat.data()[15], 16.0F);
}

TEST(MatrixTest, AccessOperatorReturnsRowsAsVectorsReferences) {
    float values[] = {1.0F, 2.0F, 3.0F, 4.0, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F, 13.0F, 14.0F, 15.0F, 16.0F};

    XE::Matrix4 mat{values};

    EXPECT_EQ(mat[0], XE::Vector4(1.0F, 2.0F, 3.0F, 4.0));
    EXPECT_EQ(mat[1], XE::Vector4(5.0F, 6.0F, 7.0F, 8.0F));
    EXPECT_EQ(mat[2], XE::Vector4(9.0F, 10.0F, 11.0F, 12.0F));
    EXPECT_EQ(mat[3], XE::Vector4(13.0F, 14.0F, 15.0F, 16.0));
}

TEST(MatrixTest, InitializerListConstructorInterpretsThemAsMatrixRows) {
    XE::Matrix3 mat{1.0F, 2.0F, 3.0F, 4.0, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F};

    EXPECT_EQ(mat[0][0], 1.0F);
    EXPECT_EQ(mat[0][1], 2.0F);
    EXPECT_EQ(mat[0][2], 3.0F);
    EXPECT_EQ(mat[1][0], 4.0F);
    EXPECT_EQ(mat[1][1], 5.0F);
    EXPECT_EQ(mat[1][2], 6.0F);
    EXPECT_EQ(mat[2][0], 7.0F);
    EXPECT_EQ(mat[2][1], 8.0F);
    EXPECT_EQ(mat[2][2], 9.0F);
}

TEST(MatrixTest, VectorConstructorOrderElementsInRowMajorOrder) {
    XE::Matrix4 mat{XE::TVector<float, 16>{1.0F, 2.0F, 3.0F, 4.0, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F, 13.0F, 14.0F, 15.0F, 16.0F}};

    EXPECT_EQ(mat[0], XE::Vector4(1.0F, 2.0F, 3.0F, 4.0));
    EXPECT_EQ(mat[1], XE::Vector4(5.0F, 6.0F, 7.0F, 8.0F));
    EXPECT_EQ(mat[2], XE::Vector4(9.0F, 10.0F, 11.0F, 12.0F));
    EXPECT_EQ(mat[3], XE::Vector4(13.0F, 14.0F, 15.0F, 16.0));
}

TEST(MatrixTest, ShouldHaveRowMajorOrder) {
    const XE::Matrix4 mat{};
    EXPECT_EQ(mat.order(), XE::MatrixOrder::RowMajor);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForTwoDimensions) {
    const auto m = XE::Matrix2::rows({XE::Vector2{1.0F, 2.0F}, XE::Vector2{3.0F, 4.0F}});

    EXPECT_EQ(m(0, 0), 1.0F);
    EXPECT_EQ(m(0, 1), 2.0F);
    EXPECT_EQ(m(1, 0), 3.0F);
    EXPECT_EQ(m(1, 1), 4.0F);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForThreeDimensions) {
    const XE::Matrix3 m = XE::Matrix3::rows({XE::Vector3{1.0F, 2.0F, 3.0F}, XE::Vector3{4.0F, 5.0F, 6.0F}, XE::Vector3{7.0F, 8.0F, 9.0F}});

    EXPECT_EQ(m(0, 0), 1.0F);
    EXPECT_EQ(m(0, 1), 2.0F);
    EXPECT_EQ(m(0, 2), 3.0F);
    EXPECT_EQ(m(1, 0), 4.0F);
    EXPECT_EQ(m(1, 1), 5.0F);
    EXPECT_EQ(m(1, 2), 6.0F);
    EXPECT_EQ(m(2, 0), 7.0F);
    EXPECT_EQ(m(2, 1), 8.0F);
    EXPECT_EQ(m(2, 2), 9.0F);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForFourDimensions) {
    const auto m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m(0, 0), 1.0F);
    EXPECT_EQ(m(0, 1), 2.0F);
    EXPECT_EQ(m(0, 2), 3.0F);
    EXPECT_EQ(m(0, 3), 4.0F);
    EXPECT_EQ(m(1, 0), 5.0F);
    EXPECT_EQ(m(1, 1), 6.0F);
    EXPECT_EQ(m(1, 2), 7.0F);
    EXPECT_EQ(m(1, 3), 8.0F);
    EXPECT_EQ(m(2, 0), 9.0F);
    EXPECT_EQ(m(2, 1), 10.0F);
    EXPECT_EQ(m(2, 2), 11.0F);
    EXPECT_EQ(m(2, 3), 12.0F);
    EXPECT_EQ(m(3, 0), 13.0F);
    EXPECT_EQ(m(3, 1), 14.0F);
    EXPECT_EQ(m(3, 2), 15.0F);
    EXPECT_EQ(m(3, 3), 16.0F);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForFourDimensionsRowVector) {
    const auto m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m(0, 0), 1.0F);
    EXPECT_EQ(m(0, 1), 2.0F);
    EXPECT_EQ(m(0, 2), 3.0F);
    EXPECT_EQ(m(0, 3), 4.0F);
    EXPECT_EQ(m(1, 0), 5.0F);
    EXPECT_EQ(m(1, 1), 6.0F);
    EXPECT_EQ(m(1, 2), 7.0F);
    EXPECT_EQ(m(1, 3), 8.0F);
    EXPECT_EQ(m(2, 0), 9.0F);
    EXPECT_EQ(m(2, 1), 10.0F);
    EXPECT_EQ(m(2, 2), 11.0F);
    EXPECT_EQ(m(2, 3), 12.0F);
    EXPECT_EQ(m(3, 0), 13.0F);
    EXPECT_EQ(m(3, 1), 14.0F);
    EXPECT_EQ(m(3, 2), 15.0F);
    EXPECT_EQ(m(3, 3), 16.0F);
}

TEST(MatrixTest, DeterminantShouldComputeTheMatrixDeterminantCorrectly) {
    const auto matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    // matrix determinant
    EXPECT_FLOAT_EQ(XE::determinant(XE::Matrix4::zero()), 0.0F);
    EXPECT_FLOAT_EQ(XE::determinant(XE::mat4Identity()), 1.0F);
    EXPECT_FLOAT_EQ(XE::determinant(matA), -32.0F);
}

TEST(MatrixTest, ComparisonOperatorsShouldCheckMatrixComponents) {
    const XE::Matrix4 m1 = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    const XE::Matrix4 m2 = XE::Matrix4::rows(
        {XE::Vector4{16.0F, 15.0F, 14.0F, 13.0F}, XE::Vector4{12.0F, 11.0F, 10.0F, 9.0F}, XE::Vector4{8.0F, 7.0F, 6.0F, 5.0F}, XE::Vector4{4.0F, 3.0F, 2.0F, 1.0F}}
    );

    EXPECT_EQ(m1, m1);
    EXPECT_EQ(m2, m2);
    EXPECT_NE(m1, m2);
    EXPECT_NE(m2, m1);
}

TEST(MatrixTest, AdditionAddsEveryFieldOneToOne) {
    const auto matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    const auto matB = XE::Matrix4::rows(
        {XE::Vector4{-3.0F, 1.0F, 5.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, -1.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, 1.0F, -2.0F}, XE::Vector4{1.0F, -1.0F, -3.0F, -1.0F}}
    );

    const auto matAddResult = XE::Matrix4::rows(
        {XE::Vector4{-2.0F, 3.0F, 6.0F, 1.0F}, XE::Vector4{3.0F, 3.0F, -4.0F, 0.0F}, XE::Vector4{-2.0F, 4.0F, 2.0F, -2.0F}, XE::Vector4{3.0F, -2.0F, -3.0F, -2.0F}}
    );

    // addition
    EXPECT_EQ(matA, +matA);
    EXPECT_EQ(matB, +matB);

    EXPECT_EQ(matA + XE::Matrix4::zero(), matA);
    EXPECT_EQ(matB + XE::Matrix4::zero(), matB);

    EXPECT_EQ(matAddResult, matA + matB);
    EXPECT_EQ(matAddResult, matB + matA);

    EXPECT_EQ(matAddResult, ((+matA) += matB));
    EXPECT_EQ(matAddResult, ((+matB) += matA));
}

TEST(MatrixTest, SubtractionSubtractsEveryFieldOneToOne) {
    const auto matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    const auto matNegA = XE::Matrix4::rows(
        {XE::Vector4{-1.0F, -2.0F, -1.0F, -0.0F}, XE::Vector4{-2.0F, -1.0F, 3.0F, 1.0F}, XE::Vector4{3.0F, -2.0F, -1.0F, -0.0F}, XE::Vector4{-2.0F, 1.0F, -0.0F, 1.0F}}
    );

    const auto matB = XE::Matrix4::rows(
        {XE::Vector4{-3.0F, 1.0F, 5.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, -1.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, 1.0F, -2.0F}, XE::Vector4{1.0F, -1.0F, -3.0F, -1.0F}}
    );

    const auto matSubResult = XE::Matrix4::rows(
        {XE::Vector4{4.0F, 1.0F, -4.0F, -1.0F}, XE::Vector4{1.0F, -1.0F, -2.0F, -2.0F}, XE::Vector4{-4.0F, 0.0F, 0.0F, 2.0F}, XE::Vector4{1.0F, 0.0F, 3.0F, 0.0F}}
    );

    // subtraction
    EXPECT_EQ(matNegA, -matA);
    EXPECT_EQ(matA - XE::Matrix4::zero(), matA);
    EXPECT_EQ(matB - XE::Matrix4::zero(), matB);

    EXPECT_EQ(XE::Matrix4::zero() - matA, -matA);
    EXPECT_EQ(XE::Matrix4::zero() - matB, -matB);

    EXPECT_EQ(matA - matB, matSubResult);
    EXPECT_EQ(matB - matA, -matSubResult);

    EXPECT_EQ(matSubResult, ((+matA) -= matB));
    EXPECT_EQ(-matSubResult, ((+matB) -= matA));
}

TEST(MatrixTest, ScalarMultiplicationMultipliesEveryFieldWithAnScalar) {
    const auto matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    // scalar multiplication
    EXPECT_EQ(matA * -1.0F, -matA);
    EXPECT_EQ(matA * -1.0F, -1.0F * matA);
    EXPECT_EQ(matA * 1.0F, matA);
    EXPECT_EQ(matA * 1.0F, 1.0F * matA);
}

TEST(MatrixTest, MatrixMultiplicationPerformsADotProductBetweenRowAndColumnsForEachResultField) {
    const auto matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    const auto matB = XE::Matrix4::rows(
        {XE::Vector4{-3.0F, 1.0F, 5.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, -1.0F, 1.0F}, XE::Vector4{1.0F, 2.0F, 1.0F, -2.0F}, XE::Vector4{1.0F, -1.0F, -3.0F, -1.0F}}
    );

    const XE::Matrix4 matMulResult = XE::Matrix4::rows(
        {XE::Vector4{0.0F, 7.0F, 4.0F, 1.0F}, XE::Vector4{-9.0F, -1.0F, 9.0F, 10.0F}, XE::Vector4{12.0F, 3.0F, -16.0F, -3.0F}, XE::Vector4{-8.0F, 1.0F, 14.0F, 2.0F}}
    );

    // matrix multiplication
    EXPECT_EQ(XE::Matrix4::zero(), XE::Matrix4::zero() * XE::Matrix4::zero());
    EXPECT_EQ(XE::Matrix4::zero(), XE::mat4Identity() * XE::Matrix4::zero());
    EXPECT_EQ(XE::mat4Identity(), XE::mat4Identity() * XE::mat4Identity());

    EXPECT_EQ(matA, matA * XE::mat4Identity());
    EXPECT_EQ(matA, XE::mat4Identity() * matA);

    EXPECT_EQ(matMulResult, matA * matB);
    EXPECT_EQ(matMulResult, ((+matA) *= matB));
}

TEST(MatrixTest, TransposeShouldSwapRowsAndColumns) {
    const auto mi = XE::mat4Identity();
    const auto m0 = XE::Matrix4::zero();

    EXPECT_EQ(mi, transpose(mi));
    EXPECT_EQ(m0, transpose(m0));

    const auto m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(
        transpose(m),
        XE::Matrix4::rows(
            {XE::Vector4{1.0F, 5.0F, 9.0F, 13.0F}, XE::Vector4{2.0F, 6.0F, 10.0F, 14.0F}, XE::Vector4{3.0F, 7.0F, 11.0F, 15.0F}, XE::Vector4{4.0F, 8.0F, 12.0F, 16.0F}}
        )
    );
}

TEST(MatrixTest, InverseShouldComputeTheMatrixInverseMultiplicative) {
    const auto invMatA = XE::Matrix4::rows(
        {XE::Vector4{0.25000F, 0.000F, -0.25000F, 0.000},
         XE::Vector4{0.28125F, 0.125F, 0.09375F, -0.125},
         XE::Vector4{0.18750F, -0.250F, 0.06250F, 0.250},
         XE::Vector4{0.21875F, -0.125F, -0.59375F, -0.875}}
    );

    const XE::Matrix4 matA = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, 1.0F, -3.0F, -1.0F}, XE::Vector4{-3.0F, 2.0F, 1.0F, 0.0F}, XE::Vector4{2.0F, -1.0F, 0.0F, -1.0F}}
    );

    auto mi = XE::mat4Identity();
    auto detMatA = -32.0F;

    EXPECT_EQ(mi, inverse(mi));
    EXPECT_EQ(invMatA, inverse(matA, detMatA));
    EXPECT_EQ(invMatA, inverse(matA));
}

TEST(MatrixTest, GetColumnShouldExtractACertainColumnFromTheMatrixAsVector) {
    const XE::Matrix4 m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m.getColumn(0), XE::Vector4(1.0F, 5.0F, 9.0F, 13.0F));
    EXPECT_EQ(m.getColumn(1), XE::Vector4(2.0F, 6.0F, 10.0F, 14.0F));
    EXPECT_EQ(m.getColumn(2), XE::Vector4(3.0F, 7.0F, 11.0F, 15.0F));
    EXPECT_EQ(m.getColumn(3), XE::Vector4(4.0F, 8.0F, 12.0F, 16.0F));
}

TEST(MatrixTest, SetColumnShouldChangeCorrectlyACertainColumnInTheMatrix) {
    const auto m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    const auto m1 = XE::Matrix4(m).setColumn(0, XE::Vector4{4.0F, 3.0F, 2.0F, 1.0F});
    const auto m1_result = XE::Matrix4::rows(
        {XE::Vector4{4.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{3.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{2.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{1.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m1, m1_result);
}

TEST(MatrixTest, GetRowShouldExtractACertainRowFromTheMatrixAsAVector) {
    const XE::Matrix4 m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 2.0F, 3.0F, 4.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(5.0F, 6.0F, 7.0F, 8.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(9.0F, 10.0F, 11.0F, 12.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(13.0F, 14.0F, 15.0F, 16.0F));
}

TEST(MatrixTest, GetSubMatrixShouldExtractASmallerMatrixFromAnother) {
    const XE::Matrix4 m = XE::Matrix4::rows(
        {XE::Vector4{1.0F, 2.0F, 3.0F, 4.0F}, XE::Vector4{5.0F, 6.0F, 7.0F, 8.0F}, XE::Vector4{9.0F, 10.0F, 11.0F, 12.0F}, XE::Vector4{13.0F, 14.0F, 15.0F, 16.0F}}
    );

    EXPECT_EQ(m.getSubMatrix(0, 0), XE::Matrix3::rows({XE::Vector3(6.0F, 7.0F, 8.0F), XE::Vector3(10.0F, 11.0F, 12.0F), XE::Vector3(14.0F, 15.0F, 16.0F)}));

    EXPECT_EQ(m.getSubMatrix(0, 1), XE::Matrix3::rows({XE::Vector3(5.0F, 7.0F, 8.0F), XE::Vector3(9.0F, 11.0F, 12.0F), XE::Vector3(13.0F, 15.0F, 16.0F)}));

    EXPECT_EQ(m.getSubMatrix(1, 0), XE::Matrix3::rows({XE::Vector3(2.0F, 3.0F, 4.0F), XE::Vector3(10.0F, 11.0F, 12.0F), XE::Vector3(14.0F, 15.0F, 16.0F)}));

    EXPECT_EQ(m.getSubMatrix(3, 3), XE::Matrix3::rows({XE::Vector3(1.0F, 2.0F, 3.0F), XE::Vector3(5.0F, 6.0F, 7.0F), XE::Vector3(9.0F, 10.0F, 11.0F)}));

    EXPECT_EQ(
        m.getSubMatrix(3, 0),
        XE::Matrix3::rows({
            XE::Vector3(2.0F, 3.0F, 4.0F),
            XE::Vector3(6.0F, 7.0F, 8.0F),
            XE::Vector3(10.0F, 11.0F, 12.0F),
        })
    );
}

TEST(MatrixTest, MatrixVectorMultiplyOperationShouldTransformTheVectorByTheRight) {
    const XE::Matrix3 m = XE::Matrix3::rows({XE::Vector3{1.0F, -1.0F, 1.0F}, XE::Vector3{-1.0F, 1.0F, -1.0F}, XE::Vector3{1.0F, 0.0F, 1.0F}});

    EXPECT_EQ(m * XE::Vector3(0.0F, 0.0F, 0.0F), XE::Vector3(0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m * XE::Vector3(1.0F, 1.0F, 1.0F), XE::Vector3(1.0F, -1.0F, 2.0F));
    EXPECT_EQ(m * XE::Vector3(-1.0F, -1.0F, -1.0F), XE::Vector3(-1.0F, 1.0F, -2.0F));
}

TEST(MatrixTest, ZeroStaticFunctionShouldCreateAValidZeroMatrix) {
    EXPECT_EQ(
        XE::Matrix4::zero(),
        XE::Matrix4::rows({XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F)})
    );

    EXPECT_EQ(XE::Matrix3::zero(), XE::Matrix3::rows({XE::Vector3(0.0F, 0.0F, 0.0F), XE::Vector3(0.0F, 0.0F, 0.0F), XE::Vector3(0.0F, 0.0F, 0.0F)}));

    EXPECT_EQ(XE::Matrix2::zero(), XE::Matrix2::rows({XE::Vector2(0.0F, 0.0F), XE::Vector2(0.0F, 0.0F)}));

    const auto mzero = XE::Matrix4::zero();

    EXPECT_EQ(mzero * XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(mzero * XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(mzero * XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(mzero * XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
}

TEST(MatrixTest, IdentityStaticFunctionShouldCreateAValidIdentityMatrix) {
    EXPECT_EQ(
        XE::mat4Identity(),
        XE::Matrix4::rows({XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F)})
    );

    EXPECT_EQ(XE::mat3Identity(), XE::Matrix3::rows({XE::Vector3(1.0F, 0.0F, 0.0F), XE::Vector3(0.0F, 1.0F, 0.0F), XE::Vector3(0.0F, 0.0F, 1.0F)}));

    EXPECT_EQ(XE::mat2Identity(), XE::Matrix2::rows({XE::Vector2(1.0F, 0.0F), XE::Vector2(0.0F, 1.0F)}));

    const auto mid = XE::mat4Identity();
    EXPECT_EQ(mid * XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(mid * XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(mid * XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(mid * XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(mid * XE::Vector4(1.0F, 2.0F, -3.0F, 4.0F), XE::Vector4(1.0F, 2.0F, -3.0F, 4.0F));
}

TEST(MatrixTest, ScaleStaticFunctionShouldCreateAValidScalingMatrix) {
    EXPECT_EQ(
        XE::mat4Scaling({1.0F, 2.0F, 3.0F, 4.0F}),
        XE::Matrix4::rows({XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 2.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 3.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 4.0F)})
    );

    EXPECT_EQ(XE::mat3Scaling({1.0F, 2.0F, 3.0F}), XE::Matrix3::rows({XE::Vector3(1.0F, 0.0F, 0.0F), XE::Vector3(0.0F, 2.0F, 0.0F), XE::Vector3(0.0F, 0.0F, 3.0F)}));

    EXPECT_EQ(XE::mat2Scaling({1.0F, 2.0F}), XE::Matrix2::rows({XE::Vector2(1.0F, 0.0F), XE::Vector2(0.0F, 2.0F)}));

    const auto m_s1 = XE::mat4Scaling({1.0F, 2.0F, 3.0F, 1.0F});
    EXPECT_EQ(m_s1 * XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m_s1 * XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m_s1 * XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 2.0F, 0.0F, 0.0F));
    EXPECT_EQ(m_s1 * XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 3.0F, 0.0F));
    EXPECT_EQ(m_s1 * XE::Vector4(1.0F, 2.0F, -3.0F, 4.0F), XE::Vector4(1.0F, 4.0F, -9.0F, 4.0F));
}

TEST(MatrixTest, CreateTranslationStaticFunctionShouldCreateAValidTranslateMatrix) {
    const auto m1 = XE::mat4Translation({2.0F, 3.0F, 4.0F});

    EXPECT_EQ(m1.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 2.0F));
    EXPECT_EQ(m1.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 3.0F));
    EXPECT_EQ(m1.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 4.0F));
    EXPECT_EQ(m1.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));
}

TEST(MatrixTest, RotateXStaticFunctionShouldCreateAXAxisRotationMatrix) {
    XE::Matrix4 m;
    float sin;
    float cos;

    m = XE::mat4RotationX(0.0F);
    sin = std::sin(0.0F);
    cos = std::cos(0.0F);
    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationX(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, cos, -sin, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, sin, cos, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationX(2.0F * XE::pi<float>);
    sin = std::sin(2.0F * XE::pi<float>);
    cos = std::cos(2.0F * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, cos, -sin, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, sin, cos, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));
}

TEST(MatrixTest, RotateYStaticFunctionShouldCreateAWellConstructedRotationMatrix) {
    XE::Matrix4 m;
    float sin;
    float cos;

    m = XE::mat4RotationY(0.0F);
    sin = std::sin(0.0F);
    cos = std::cos(0.0F);
    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationY(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(cos, 0.0F, sin, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(-sin, 0.0F, cos, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationY(2.0F * XE::pi<float>);
    sin = std::sin(2.0F * XE::pi<float>);
    cos = std::cos(2.0F * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(cos, 0.0F, sin, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(-sin, 0.0F, cos, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));
}

TEST(MatrixTest, RotateZStaticFunctionShouldCreateAWellConstructedRotationMatrix) {
    XE::Matrix4 m;
    float sin;
    float cos;

    m = XE::mat4RotationZ(0.0F);
    sin = std::sin(0.0F);
    cos = std::cos(0.0F);
    EXPECT_EQ(m.getRow(0), XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationZ(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(cos, -sin, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(sin, cos, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));

    m = XE::mat4RotationZ(2.0F * XE::pi<float>);
    sin = std::sin(2.0F * XE::pi<float>);
    cos = std::cos(2.0F * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4(cos, -sin, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(1), XE::Vector4(sin, cos, 0.0F, 0.0F));
    EXPECT_EQ(m.getRow(2), XE::Vector4(0.0F, 0.0F, 1.0F, 0.0F));
    EXPECT_EQ(m.getRow(3), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F));
}

TEST(MatrixTest, RotateWithFixedAxisShouldMatchTheCorrespondingRotateXYZStaticMethods) {
    EXPECT_EQ(XE::mat4Rotation(0.0F, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4Identity());
    EXPECT_EQ(XE::mat4Rotation(0.0F, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4Identity());
    EXPECT_EQ(XE::mat4Rotation(0.0F, XE::Vector3(-1.0F, 0.0F, 0.0F)), XE::mat4Identity());
    EXPECT_EQ(XE::mat4Rotation(0.0F, XE::Vector3(0.0F, -1.0F, 0.0F)), XE::mat4Identity());
    EXPECT_EQ(XE::mat4Rotation(0.0F, XE::Vector3(0.0F, 0.0F, -1.0F)), XE::mat4Identity());

    EXPECT_EQ(XE::mat4Rotation(0.0F * XE::pi<float>, XE::Vector3(1.0F, 0.0F, 0.0F)), XE::mat4RotationX(0.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(0.5F * XE::pi<float>, XE::Vector3(1.0F, 0.0F, 0.0F)), XE::mat4RotationX(0.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.0F * XE::pi<float>, XE::Vector3(1.0F, 0.0F, 0.0F)), XE::mat4RotationX(1.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.5F * XE::pi<float>, XE::Vector3(1.0F, 0.0F, 0.0F)), XE::mat4RotationX(1.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(2.0F * XE::pi<float>, XE::Vector3(1.0F, 0.0F, 0.0F)), XE::mat4RotationX(2.0F * XE::pi<float>));

    EXPECT_EQ(XE::mat4Rotation(0.0F * XE::pi<float>, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4RotationY(0.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(0.5F * XE::pi<float>, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4RotationY(0.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.0F * XE::pi<float>, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4RotationY(1.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.5F * XE::pi<float>, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4RotationY(1.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(2.0F * XE::pi<float>, XE::Vector3(0.0F, 1.0F, 0.0F)), XE::mat4RotationY(2.0F * XE::pi<float>));

    EXPECT_EQ(XE::mat4Rotation(0.0F * XE::pi<float>, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4RotationZ(0.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(0.5F * XE::pi<float>, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4RotationZ(0.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.0F * XE::pi<float>, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4RotationZ(1.0F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(1.5F * XE::pi<float>, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4RotationZ(1.5F * XE::pi<float>));
    EXPECT_EQ(XE::mat4Rotation(2.0F * XE::pi<float>, XE::Vector3(0.0F, 0.0F, 1.0F)), XE::mat4RotationZ(2.0F * XE::pi<float>));
}

TEST(MatrixTest, LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtThOriginLookingAtTheZAxisAndYOrientation_ShouldGenerateAnIdentityMatrix) {
    const auto lookAt = XE::mat4LookAtRH({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, -1.0F}, {0.0F, 1.0F, 0.0F});
    EXPECT_EQ(lookAt, XE::mat4Identity());
}

TEST(
    MatrixTest,
    LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtTheOrigin_LookingAtTheZAxisAndYOrientation_ShouldGenerateAnPseudoIdentityMatrixWithSomeNegativeUnitAxis
) {
    const auto lookAt = XE::mat4LookAtRH({0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F});

    EXPECT_EQ(
        lookAt,
        XE::Matrix4::rows({XE::Vector4(-1.0F, 0.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F), XE::Vector4(0.0F, 0.0F, -1.0F, 0.0F), XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F)})
    );
}

TEST(MatrixTest, LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtTheMinus10ZLookingAtTheZAxisAndYOrientationShouldGenerateATranslationMatrix) {
    const auto lookAt1 = XE::mat4LookAtRH({0.0F, 0.0F, 10.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 0.0F});
    EXPECT_EQ(lookAt1, XE::mat4Translation(XE::Vector3(0.0F, 0.0F, 10.0F)));

    const auto lookAt2 = XE::mat4LookAtRH({0.0F, 0.0F, 10.0F}, {0.0F, 0.0F, -1.0F}, {0.0F, 1.0F, 0.0F});
    EXPECT_EQ(lookAt2, XE::mat4Translation(XE::Vector3(0.0F, 0.0F, 10.0F)));
}

TEST(MatrixTest, createPerspectiveShouldCreateAPerspectiveTransformationMatrix) {
    const auto m1 = XE::mat4Perspective(XE::radians(60.0F), (320.0F / 240.0F), 0.1F, 100.0F);
    EXPECT_EQ(
        m1,
        XE::Matrix4::rows(
            {XE::Vector4(1.299038170F, 0.000000000F, 0.000000000F, 0.000000000F),
             XE::Vector4(0.000000000F, 1.73205090F, 0.000000000F, 0.000000000F),
             XE::Vector4(0.000000000F, 0.000000000F, -1.002002001F, -0.200200200F),
             XE::Vector4(0.000000000F, 0.000000000F, -1.000000000F, 0.000000000F)}
        )
    );

    const auto m2 = XE::mat4Perspective(XE::radians(120.0F), 1.33333F, 0.1F, 100.0F);
    EXPECT_EQ(
        m2,
        XE::Matrix4::rows(
            {XE::Vector4(0.433013767F, 0.000000000F, 0.000000000F, 0.000000000F),
             XE::Vector4(0.000000000F, 0.577350259F, 0.000000000F, 0.000000000F),
             XE::Vector4(0.000000000F, 0.000000000F, -1.002002001F, -0.200200200F),
             XE::Vector4(0.000000000F, 0.000000000F, -1.000000000F, 0.000000000F)}
        )
    );
}

TEST(MatrixTest, CreateOrthographicShouldCreateAOrthographicTransformationMatrix) {
    const auto m1 = XE::mat4Ortho({-1.0F, -1.0F, -1.0F}, {1.0F, 1.0F, 1.0F});
    EXPECT_EQ(
        m1,
        XE::Matrix4::rows({
            XE::Vector4(1.0F, 0.0F, 0.0F, 0.0F),
            XE::Vector4(0.0F, 1.0F, 0.0F, 0.0F),
            XE::Vector4(0.0F, 0.0F, -1.0F, 0.0F),
            XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F),
        })
    );

    const auto m2 = XE::mat4Ortho({-2.0F, -2.0F, -2.0F}, {2.0F, 2.0F, 2.0F});
    EXPECT_EQ(
        m2,
        XE::Matrix4::rows({
            XE::Vector4(0.5F, 0.0F, 0.0F, 0.0F),
            XE::Vector4(0.0F, 0.5F, 0.0F, 0.0F),
            XE::Vector4(0.0F, 0.0F, -0.5F, 0.0F),
            XE::Vector4(0.0F, 0.0F, 0.0F, 1.0F),
        })
    );

    const auto m3 = XE::mat4Ortho({-0.5F, -1.5F, 0.0F}, {2.5F, 3.5F, 100.0F});
    EXPECT_EQ(
        m3,
        XE::Matrix4::rows(
            {XE::Vector4(0.666666687F, 0.000000000F, 0.000000000F, -0.666666687F),
             XE::Vector4(0.000000000F, 0.400000006F, 0.000000000F, -0.400000006F),
             XE::Vector4(0.000000000F, 0.000000000F, -0.020000000F, -1.000000000F),
             XE::Vector4(0.000000000F, 0.000000000F, 0.000000000F, 1.000000000F)}
        )
    );

    const auto m4 = XE::mat4Ortho({-0.5F, -1.5F, 100.0F}, {2.5F, 3.5F, -50.0F});
    EXPECT_EQ(
        m4,
        XE::Matrix4::rows(
            {XE::Vector4(0.666666687F, 0.000000000F, 0.000000000F, -0.666666687F),
             XE::Vector4(0.000000000F, 0.400000006F, 0.000000000F, -0.400000006F),
             XE::Vector4(0.000000000F, 0.000000000F, 0.013333334F, 0.333333343F),
             XE::Vector4(0.000000000F, 0.000000000F, 0.000000000F, 1.000000000F)}
        )
    );
}
