
#include "Common.h"

#include <xe/math/Common.h>
#include <xe/math/Matrix.h>
#include <xe/math/Vector.h>


TEST(MatrixTest, ShouldHaveRowMajorOrder) {
    const auto matA = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, 1.0f, -3.0f, -1.0f},
        XE::Vector4f{-3.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, -1.0f, 0.0f, -1.0f}
    });

    EXPECT_EQ(matA.data()[0], 1.0f);
    EXPECT_EQ(matA.data()[1], 2.0f);
    EXPECT_EQ(matA.data()[2], 1.0f);
    EXPECT_EQ(matA.data()[3], 0.0f);
    EXPECT_EQ(matA.data()[4], 2.0f);
    EXPECT_EQ(matA.data()[5], 1.0f);
    EXPECT_EQ(matA.data()[6], -3.0f);
    EXPECT_EQ(matA.data()[7], -1.0f);
    EXPECT_EQ(matA.data()[8], -3.0f);
    EXPECT_EQ(matA.data()[9], 2.0f);
    EXPECT_EQ(matA.data()[10], 1.0f);
    EXPECT_EQ(matA.data()[11], 0.0f);
    EXPECT_EQ(matA.data()[12], 2.0f);
    EXPECT_EQ(matA.data()[13], -1.0f);
    EXPECT_EQ(matA.data()[14], 0.0f);
    EXPECT_EQ(matA.data()[15], -1.0f);
}

TEST(MatrixTest, DeterminantShouldComputeTheMatrixDeterminantCorrectly) {
    const auto matA = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, 1.0f, -3.0f, -1.0f},
        XE::Vector4f{-3.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, -1.0f, 0.0f, -1.0f}
    });

    // matrix determinant
    EXPECT_EQ(XE::determinant(XE::Matrix4f::zero()), 0.0f);
    EXPECT_EQ(XE::determinant(XE::Matrix4f::identity()), 1.0f);
    EXPECT_EQ(XE::determinant(matA), -32.0f);
}

// TODO: Refactor each operator into its own test case
TEST(MatrixTest, ArithmeticOperatorsShouldBehaveAccordingToTheirCorrespondingMathematicalDefinitions) {
    const auto matA = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, 1.0f, -3.0f, -1.0f},
        XE::Vector4f{-3.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, -1.0f, 0.0f, -1.0f}
    });

    const auto matNegA = XE::Matrix4f::rows({
        XE::Vector4f{-1.0f, -2.0f, -1.0f, -0.0f},
        XE::Vector4f{-2.0f, -1.0f, 3.0f, 1.0f},
        XE::Vector4f{3.0f, -2.0f, -1.0f, -0.0f},
        XE::Vector4f{-2.0f, 1.0f, -0.0f, 1.0f}
    });

    const auto matB = XE::Matrix4f::rows({
        XE::Vector4f{-3.0f, 1.0f, 5.0f, 1.0f},
        XE::Vector4f{1.0f, 2.0f, -1.0f, 1.0f},
        XE::Vector4f{1.0f, 2.0f, 1.0f, -2.0f},
        XE::Vector4f{1.0f, -1.0f, -3.0f, -1.0f}
    });

    const auto matAddResult = XE::Matrix4f::rows({
        XE::Vector4f{-2.0f,  3.0f,  6.0f,  1.0f},
        XE::Vector4f{ 3.0f,  3.0f, -4.0f,  0.0f},
        XE::Vector4f{-2.0f,  4.0f,  2.0f, -2.0f},
        XE::Vector4f{ 3.0f, -2.0f, -3.0f, -2.0f}
    });

    const auto matSubResult = XE::Matrix4f::rows({
        XE::Vector4f{ 4.0f,  1.0f, -4.0f, -1.0f},
        XE::Vector4f{ 1.0f, -1.0f, -2.0f, -2.0f},
        XE::Vector4f{-4.0f,  0.0f,  0.0f,  2.0f},
        XE::Vector4f{ 1.0f,  0.0f,  3.0f,  0.0f}
    });

    const XE::Matrix4f matMulResult = XE::Matrix4f::rows({
        XE::Vector4f{  0.0f,  7.0f,  4.0f,   1.0f},
        XE::Vector4f{ -9.0f, -1.0f,  9.0f,  10.0f},
        XE::Vector4f{ 12.0f,  3.0f, -16.0f, -3.0f},
        XE::Vector4f{ -8.0f,  1.0f,  14.0f,  2.0f}
    });


    // addition
    EXPECT_EQ(matA, +matA);
    EXPECT_EQ(matB, +matB);

    EXPECT_EQ(matA + XE::Matrix4f::zero(), matA);
    EXPECT_EQ(matB + XE::Matrix4f::zero(), matB);

    EXPECT_EQ(matAddResult, matA + matB);
    EXPECT_EQ(matAddResult, matB + matA);

    EXPECT_EQ(matAddResult, ((+matA) += matB));
    EXPECT_EQ(matAddResult, ((+matB) += matA));

    // subtraction
    EXPECT_EQ(matNegA, -matA);
    EXPECT_EQ(matA - XE::Matrix4f::zero(), matA);
    EXPECT_EQ(matB - XE::Matrix4f::zero(), matB);

    EXPECT_EQ(XE::Matrix4f::zero() - matA, -matA);
    EXPECT_EQ(XE::Matrix4f::zero() - matB, -matB);

    EXPECT_EQ(matA - matB, matSubResult);
    EXPECT_EQ(matB - matA, -matSubResult);

    EXPECT_EQ(matSubResult, ((+matA) -= matB));
    EXPECT_EQ(-matSubResult, ((+matB) -= matA));

    // scalar multiplication
    EXPECT_EQ(matA * -1.0f, -matA);
    EXPECT_EQ(matA * -1.0f, -1.0f * matA);
    EXPECT_EQ(matA * 1.0f, matA);
    EXPECT_EQ(matA * 1.0f, 1.0f * matA);

    // matrix multiplication
    EXPECT_EQ(XE::Matrix4f::zero(), XE::Matrix4f::zero() * XE::Matrix4f::zero());
    EXPECT_EQ(XE::Matrix4f::zero(), XE::Matrix4f::identity() * XE::Matrix4f::zero());
    EXPECT_EQ(XE::Matrix4f::identity(), XE::Matrix4f::identity() * XE::Matrix4f::identity());

    EXPECT_EQ(matA, matA * XE::Matrix4f::identity());
    EXPECT_EQ(matA, XE::Matrix4f::identity() * matA);

    EXPECT_EQ(matMulResult, matA * matB);
    EXPECT_EQ(matMulResult, ((+matA) *= matB));
}


TEST(MatrixTest, TransposeShouldSwapRowsAndColumns) {
    const auto mi = XE::Matrix4f::identity();
    const auto m0 = XE::Matrix4f::zero();

    EXPECT_EQ(mi, transpose(mi));
    EXPECT_EQ(m0, transpose(m0));

    const auto m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(transpose(m), XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 5.0f, 9.0f, 13.0f},
        XE::Vector4f{2.0f, 6.0f, 10.0f,14.0f},
        XE::Vector4f{3.0f, 7.0f, 11.0f, 15.0f},
        XE::Vector4f{4.0f, 8.0f, 12.0f, 16.0f}
    }));
}

TEST(MatrixTest, InverseShouldComputeTheMatrixInverseMultiplicative) {
    const auto invMatA = XE::Matrix4f::rows({
        XE::Vector4f{0.25000f,  0.000f, -0.25000f,  0.000},
        XE::Vector4f{0.28125f,  0.125f,  0.09375f, -0.125},
        XE::Vector4f{0.18750f, -0.250f,  0.06250f,  0.250},
        XE::Vector4f{0.21875f, -0.125f, -0.59375f, -0.875}
    });

    const XE::Matrix4f matA = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, 1.0f, -3.0f, -1.0f},
        XE::Vector4f{-3.0f, 2.0f, 1.0f, 0.0f},
        XE::Vector4f{2.0f, -1.0f, 0.0f, -1.0f}
    });

    auto mi = XE::Matrix4f::identity();
    auto detMatA = -32.0f;

    EXPECT_EQ(mi, inverse(mi));
    EXPECT_EQ(invMatA, inverse(matA, detMatA));
    EXPECT_EQ(invMatA, inverse(matA));
}


TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForTwoDimensions) {
    const auto m = XE::Matrix2f::rows({
        XE::Vector2f{1.0f, 2.0f},
        XE::Vector2f{3.0f, 4.0f}
    });

    EXPECT_EQ(m(0, 0), 1.0f); EXPECT_EQ(m(0, 1), 2.0f);
    EXPECT_EQ(m(1, 0), 3.0f); EXPECT_EQ(m(1, 1), 4.0f);
}


TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForThreeDimensions) {
    const XE::Matrix3f m = XE::Matrix3f::rows({
        XE::Vector3f{1.0f, 2.0f, 3.0f},
        XE::Vector3f{4.0f, 5.0f, 6.0f},
        XE::Vector3f{7.0f, 8.0f, 9.0f}
    });

    EXPECT_EQ(m(0, 0), 1.0f); EXPECT_EQ(m(0, 1), 2.0f); EXPECT_EQ(m(0, 2), 3.0f);
    EXPECT_EQ(m(1, 0), 4.0f); EXPECT_EQ(m(1, 1), 5.0f); EXPECT_EQ(m(1, 2), 6.0f);
    EXPECT_EQ(m(2, 0), 7.0f); EXPECT_EQ(m(2, 1), 8.0f); EXPECT_EQ(m(2, 2), 9.0f);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForFourDimensions) {
    const auto m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(m(0, 0), 1.0f); EXPECT_EQ(m(0, 1), 2.0f); EXPECT_EQ(m(0, 2), 3.0f); EXPECT_EQ(m(0, 3), 4.0f);
    EXPECT_EQ(m(1, 0), 5.0f); EXPECT_EQ(m(1, 1), 6.0f); EXPECT_EQ(m(1, 2), 7.0f); EXPECT_EQ(m(1, 3), 8.0f);
    EXPECT_EQ(m(2, 0), 9.0f); EXPECT_EQ(m(2, 1), 10.0f); EXPECT_EQ(m(2, 2), 11.0f); EXPECT_EQ(m(2, 3), 12.0f);
    EXPECT_EQ(m(3, 0), 13.0f); EXPECT_EQ(m(3, 1), 14.0f); EXPECT_EQ(m(3, 2), 15.0f); EXPECT_EQ(m(3, 3), 16.0f);
}

TEST(MatrixTest, ConstructorShouldSetupTheMatrixComponentsCorrectlyForFourDimensionsRowVector) {
    const auto m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(m(0, 0), 1.0f); EXPECT_EQ(m(0, 1), 2.0f); EXPECT_EQ(m(0, 2), 3.0f); EXPECT_EQ(m(0, 3), 4.0f);
    EXPECT_EQ(m(1, 0), 5.0f); EXPECT_EQ(m(1, 1), 6.0f); EXPECT_EQ(m(1, 2), 7.0f); EXPECT_EQ(m(1, 3), 8.0f);
    EXPECT_EQ(m(2, 0), 9.0f); EXPECT_EQ(m(2, 1), 10.0f); EXPECT_EQ(m(2, 2), 11.0f); EXPECT_EQ(m(2, 3), 12.0f);
    EXPECT_EQ(m(3, 0), 13.0f); EXPECT_EQ(m(3, 1), 14.0f); EXPECT_EQ(m(3, 2), 15.0f); EXPECT_EQ(m(3, 3), 16.0f);
}


TEST(MatrixTest, ComparisonOperatorsShouldCheckMatrixComponents) {
    const XE::Matrix4f m1 = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    const XE::Matrix4f m2 = XE::Matrix4f::rows({
        XE::Vector4f{16.0f, 15.0f, 14.0f, 13.0f},
        XE::Vector4f{12.0f, 11.0f, 10.0f, 9.0f},
        XE::Vector4f{8.0f, 7.0f, 6.0f, 5.0f},
        XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f}
    });

    EXPECT_EQ(m1, m1);
    EXPECT_EQ(m2, m2);
    EXPECT_NE(m1, m2);
    EXPECT_NE(m2, m1);
}


TEST(MatrixTest, GetColumnShouldExtractACertainColumnFromTheMatrixAsVector) {
    const XE::Matrix4f m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(m.getColumn(0), XE::Vector4f(1.0f, 5.0f, 9.0f, 13.0f));
    EXPECT_EQ(m.getColumn(1), XE::Vector4f(2.0f, 6.0f, 10.0f, 14.0f));
    EXPECT_EQ(m.getColumn(2), XE::Vector4f(3.0f, 7.0f, 11.0f, 15.0f));
    EXPECT_EQ(m.getColumn(3), XE::Vector4f(4.0f, 8.0f, 12.0f, 16.0f));
}


TEST(MatrixTest, SetColumnShouldChangeCorrectlyACertainColumnInTheMatrix) {
    const auto m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}});

    const auto m1 = XE::Matrix4f(m).setColumn(0, XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f});
    const auto m1_result = XE::Matrix4f::rows({
        XE::Vector4f{4.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{3.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{2.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{1.0f, 14.0f, 15.0f, 16.0f}});

    EXPECT_EQ(m1, m1_result);
}


TEST(MatrixTest, GetRowShouldExtractACertainRowFromTheMatrixAsAVector) {
    const XE::Matrix4f m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(5.0f, 6.0f, 7.0f, 8.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(9.0f, 10.0f, 11.0f, 12.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(13.0f, 14.0f, 15.0f, 16.0f));
}

TEST(MatrixTest, GetSubMatrixShouldExtractASmallerMatrixFromAnother) {
    const XE::Matrix4f m = XE::Matrix4f::rows({
        XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
        XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
        XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
        XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
    });

    EXPECT_EQ(m.getSubMatrix(0, 0), XE::Matrix3f::rows({
        XE::Vector3f(6.0f, 7.0f, 8.0f),
        XE::Vector3f(10.0f, 11.0f, 12.0f),
        XE::Vector3f(14.0f, 15.0f, 16.0f)
    }));

    EXPECT_EQ(m.getSubMatrix(0, 1), XE::Matrix3f::rows({
        XE::Vector3f(5.0f, 7.0f, 8.0f),
        XE::Vector3f(9.0f, 11.0f, 12.0f),
        XE::Vector3f(13.0f, 15.0f, 16.0f)
    }));

    EXPECT_EQ(m.getSubMatrix(1, 0), XE::Matrix3f::rows({
        XE::Vector3f(2.0f, 3.0f, 4.0f),
        XE::Vector3f(10.0f, 11.0f, 12.0f),
        XE::Vector3f(14.0f, 15.0f, 16.0f)
    }));

    EXPECT_EQ(m.getSubMatrix(3, 3), XE::Matrix3f::rows({
        XE::Vector3f(1.0f, 2.0f, 3.0f),
        XE::Vector3f(5.0f, 6.0f, 7.0f),
        XE::Vector3f(9.0f, 10.0f, 11.0f)
    }));

    EXPECT_EQ(m.getSubMatrix(3, 0), XE::Matrix3f::rows({
        XE::Vector3f(2.0f, 3.0f, 4.0f),
        XE::Vector3f(6.0f, 7.0f, 8.0f),
        XE::Vector3f(10.0f, 11.0f, 12.0f),
    }));
}


TEST(MatrixTest, MatrixVectorMultiplyOperationShouldTransformTheVectorByTheRight) {
    const XE::Matrix3f m = XE::Matrix3f::rows({
        XE::Vector3f{1.0f, -1.0f, 1.0f},
        XE::Vector3f{-1.0f, 1.0f, -1.0f},
        XE::Vector3f{1.0f, 0.0f, 1.0f}
    });

    EXPECT_EQ(m * XE::Vector3f(0.0f, 0.0f, 0.0f), XE::Vector3f(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m * XE::Vector3f(1.0f, 1.0f, 1.0f), XE::Vector3f(1.0f, -1.0f, 2.0f));
    EXPECT_EQ(m * XE::Vector3f(-1.0f, -1.0f, -1.0f), XE::Vector3f(-1.0f, 1.0f, -2.0f));
}


TEST(MatrixTest, VectorMatrixMultiplyOperationShouldTransformTheVectorByTheLeft) {
    const auto m = XE::Matrix3f::rows({
        XE::Vector3f{1.0f, -1.0f, 1.0f},
        XE::Vector3f{-1.0f, 1.0f, -1.0f},
        XE::Vector3f{1.0f, 0.0f, 1.0f}
    });

    EXPECT_EQ(XE::Vector3f(0.0f, 0.0f, 0.0f) * m, XE::Vector3f(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(XE::Vector3f(1.0f, 1.0f, 1.0f) * m , XE::Vector3f(1.0f, 0.0f, 1.0f));
    EXPECT_EQ(XE::Vector3f(-1.0f, -1.0f, -1.0f) * m , XE::Vector3f(-1.0f, 0.0f, -1.0f));
}


TEST(MatrixTest, ZeroStaticFunctionShouldCreateAValidZeroMatrix) {
    EXPECT_EQ(XE::Matrix4f::zero(), XE::Matrix4f::rows({
        XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f)
    }));

    EXPECT_EQ(XE::Matrix3f::zero(), XE::Matrix3f::rows({
        XE::Vector3f(0.0f, 0.0f, 0.0f),
        XE::Vector3f(0.0f, 0.0f, 0.0f),
        XE::Vector3f(0.0f, 0.0f, 0.0f)
    }));

    EXPECT_EQ(XE::Matrix2f::zero(), XE::Matrix2f::rows({
        XE::Vector2f(0.0f, 0.0f),
        XE::Vector2f(0.0f, 0.0f)
    }));

    const auto mzero = XE::Matrix4f::zero();

    EXPECT_EQ(mzero * XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(mzero * XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(mzero * XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(mzero * XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
}

TEST(MatrixTest, IdentityStaticFunctionShouldCreateAValidIdentityMatrix) {
    EXPECT_EQ(XE::Matrix4f::identity(), XE::Matrix4f::rows({
        XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f)
    }));

    EXPECT_EQ(XE::Matrix3f::identity(), XE::Matrix3f::rows({
        XE::Vector3f(1.0f, 0.0f, 0.0f),
        XE::Vector3f(0.0f, 1.0f, 0.0f),
        XE::Vector3f(0.0f, 0.0f, 1.0f)
    }));

    EXPECT_EQ(XE::Matrix2f::identity(), XE::Matrix2f::rows({
        XE::Vector2f(1.0f, 0.0f),
        XE::Vector2f(0.0f, 1.0f)
    }));

    const auto mid = XE::Matrix4f::identity();
    EXPECT_EQ(mid * XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(mid * XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(mid * XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(mid * XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(mid * XE::Vector4f(1.0f, 2.0f, -3.0f, 4.0f), XE::Vector4f(1.0f, 2.0f, -3.0f, 4.0f));
}

TEST(MatrixTest, ScaleStaticFunctionShouldCreateAValidScalingMatrix) {
    EXPECT_EQ(XE::Matrix4f::scale({1.0f, 2.0f, 3.0f, 4.0f}), XE::Matrix4f::rows({
        XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 2.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 3.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 4.0f)
    }));

    EXPECT_EQ(XE::Matrix3f::scale({1.0f, 2.0f, 3.0f}), XE::Matrix3f::rows({
        XE::Vector3f(1.0f, 0.0f, 0.0f),
        XE::Vector3f(0.0f, 2.0f, 0.0f),
        XE::Vector3f(0.0f, 0.0f, 3.0f)
    }));

    EXPECT_EQ(XE::Matrix2f::scale({1.0f, 2.0f}), XE::Matrix2f::rows({
        XE::Vector2f(1.0f, 0.0f),
        XE::Vector2f(0.0f, 2.0f)
    }));

    const auto m_s1 = XE::Matrix4f::scale({1.0f, 2.0f, 3.0f, 1.0f});
    EXPECT_EQ(m_s1 * XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m_s1 * XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m_s1 * XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 2.0f, 0.0f, 0.0f));
    EXPECT_EQ(m_s1 * XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 3.0f, 0.0f));
    EXPECT_EQ(m_s1 * XE::Vector4f(1.0f, 2.0f, -3.0f, 4.0f), XE::Vector4f(1.0f, 4.0f, -9.0f, 4.0f));
}


TEST(MatrixTest, CreateTranslationStaticFunctionShouldCreateAValidTranslateMatrix) {
    const auto m1 = XE::Matrix4f::translate({2.0f, 3.0f, 4.0f, 1.0f});
    EXPECT_EQ(m1.getColumn(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m1.getColumn(1), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(m1.getColumn(2), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m1.getColumn(3), XE::Vector4f(2.0f, 3.0f, 4.0f, 1.0f));

    const auto m2 = XE::Matrix4f::translate({2.0f, -3.0f, 4.0f});
    EXPECT_EQ(m2.getColumn(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m2.getColumn(1), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(m2.getColumn(2), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m2.getColumn(3), XE::Vector4f(2.0f, -3.0f, 4.0f, 1.0f));

    EXPECT_EQ(m1 * XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), XE::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m1 * XE::Vector4f(2.0f, -2.0f, 1.0f, 1.0f), XE::Vector4f(4.0f, 1.0f, 5.0f, 1.0f));
    EXPECT_EQ(m1 * XE::Vector4f(1.0f, 0.0f, 1.0f, 1.0f), XE::Vector4f(3.0f, 3.0f, 5.0f, 1.0f));
    EXPECT_EQ(m2 * XE::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), XE::Vector4f(3.0f, -2.0f, 5.0f, 1.0f));
    EXPECT_EQ(m2 * XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f), XE::Vector4f(2.0f, -3.0f, 4.0f, 1.0f));

    EXPECT_EQ(m1 * XE::Vector4f(2.0f, -2.0f, 1.0f, 0.0f), XE::Vector4f(2.0f, -2.0f, 1.0f, 0.0f));
    EXPECT_EQ(m1 * XE::Vector4f(1.0f, 0.0f, 1.0f, 0.0f), XE::Vector4f(1.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m2 * XE::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), XE::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));
}

TEST(MatrixTest, RotateXStaticFunctionShouldCreateAXAxisRotationMatrix) {
    XE::Matrix4f m;
    float sin, cos;

    m = XE::Matrix4f::rotateX(0.0f);
    sin = std::sin(0.0f);
    cos = std::cos(0.0f);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    m = XE::Matrix4f::rotateX(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, cos, -sin, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f, sin, cos, 0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    m = XE::Matrix4f::rotateX(2.0f * XE::pi<float>);
    sin = std::sin(2.0f * XE::pi<float>);
    cos = std::cos(2.0f * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, cos, -sin, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f, sin, cos, 0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
}

TEST(MatrixTest, RotateYStaticFunctionShouldCreateAWellConstructedRotationMatrix) {
    XE::Matrix4f m;
    float sin, cos;

    m = XE::Matrix4f::rotateY(0.0f);
    sin = std::sin(0.0f);
    cos = std::cos(0.0f);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    m = XE::Matrix4f::rotateY(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(cos,  0.0f, sin,  0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, 1.0f,  0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(-sin,  0.0f,  cos,  0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f,  0.0f, 1.0f));

    m = XE::Matrix4f::rotateY(2.0f * XE::pi<float>);
    sin = std::sin(2.0f * XE::pi<float>);
    cos = std::cos(2.0f * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(cos,  0.0f, sin,  0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, 1.0f,  0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(-sin,  0.0f,  cos,  0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f,  0.0f, 1.0f));
}

TEST(MatrixTest, RotateZStaticFunctionShouldCreateAWellConstructedRotationMatrix) {
    XE::Matrix4f m;
    float sin, cos;

    m = XE::Matrix4f::rotateZ(0.0f);
    sin = std::sin(0.0f);
    cos = std::cos(0.0f);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    m = XE::Matrix4f::rotateZ(XE::pi<float>);
    sin = std::sin(XE::pi<float>);
    cos = std::cos(XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(cos, -sin, 0.0f,  0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(sin, cos,  0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f,  0.0f,  1.0f,  0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f,  0.0f, 1.0f));

    m = XE::Matrix4f::rotateZ(2.0f * XE::pi<float>);
    sin = std::sin(2.0f * XE::pi<float>);
    cos = std::cos(2.0f * XE::pi<float>);
    EXPECT_EQ(m.getRow(0), XE::Vector4f(cos, -sin, 0.0f,  0.0f));
    EXPECT_EQ(m.getRow(1), XE::Vector4f(sin, cos,  0.0f, 0.0f));
    EXPECT_EQ(m.getRow(2), XE::Vector4f(0.0f,  0.0f,  1.0f,  0.0f));
    EXPECT_EQ(m.getRow(3), XE::Vector4f(0.0f, 0.0f,  0.0f, 1.0f));
}


TEST(MatrixTest, RotateWithFixedAxisShouldMatchTheCorrespondingRotateXYZStaticMethods) {
    EXPECT_EQ(XE::Matrix4f::rotate(0.0f, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::identity());
    EXPECT_EQ(XE::Matrix4f::rotate(0.0f, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::identity());
    EXPECT_EQ(XE::Matrix4f::rotate(0.0f, XE::Vector3f(-1.0f, 0.0f, 0.0f)), XE::Matrix4f::identity());
    EXPECT_EQ(XE::Matrix4f::rotate(0.0f, XE::Vector3f(0.0f, -1.0f, 0.0f)), XE::Matrix4f::identity());
    EXPECT_EQ(XE::Matrix4f::rotate(0.0f, XE::Vector3f(0.0f, 0.0f, -1.0f)), XE::Matrix4f::identity());

    EXPECT_EQ(XE::Matrix4f::rotate(0.0f * XE::pi<float>, XE::Vector3f(1.0f, 0.0f, 0.0f)), XE::Matrix4f::rotateX(0.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(0.5f * XE::pi<float>, XE::Vector3f(1.0f, 0.0f, 0.0f)), XE::Matrix4f::rotateX(0.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.0f * XE::pi<float>, XE::Vector3f(1.0f, 0.0f, 0.0f)), XE::Matrix4f::rotateX(1.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.5f * XE::pi<float>, XE::Vector3f(1.0f, 0.0f, 0.0f)), XE::Matrix4f::rotateX(1.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(2.0f * XE::pi<float>, XE::Vector3f(1.0f, 0.0f, 0.0f)), XE::Matrix4f::rotateX(2.0f * XE::pi<float>));

    EXPECT_EQ(XE::Matrix4f::rotate(0.0f * XE::pi<float>, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::rotateY(0.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(0.5f * XE::pi<float>, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::rotateY(0.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.0f * XE::pi<float>, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::rotateY(1.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.5f * XE::pi<float>, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::rotateY(1.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(2.0f * XE::pi<float>, XE::Vector3f(0.0f, 1.0f, 0.0f)), XE::Matrix4f::rotateY(2.0f * XE::pi<float>));

    EXPECT_EQ(XE::Matrix4f::rotate(0.0f * XE::pi<float>, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::rotateZ(0.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(0.5f * XE::pi<float>, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::rotateZ(0.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.0f * XE::pi<float>, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::rotateZ(1.0f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(1.5f * XE::pi<float>, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::rotateZ(1.5f * XE::pi<float>));
    EXPECT_EQ(XE::Matrix4f::rotate(2.0f * XE::pi<float>, XE::Vector3f(0.0f, 0.0f, 1.0f)), XE::Matrix4f::rotateZ(2.0f * XE::pi<float>));
}

TEST(MatrixTest, LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtThOriginLookingAtTheZAxisAndYOrientation_ShouldGenerateAnIdentityMatrix) {
    const auto lookAt = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
    EXPECT_EQ(lookAt, XE::Matrix4f::identity());
}

TEST(MatrixTest, LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtTheOrigin_LookingAtTheZAxisAndYOrientation_ShouldGenerateAnPseudoIdentityMatrixWithSomeNegativeUnitAxis) {
    const auto lookAt = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

    EXPECT_EQ(lookAt, XE::Matrix4f::rows({
        XE::Vector4f( -1.0f, 0.0f, 0.0f, 0.0f ),
        XE::Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ),
        XE::Vector4f( 0.0f, 0.0f, -1.0f, 0.0f ),
        XE::Vector4f( 0.0f, 0.0f, 0.0f, 1.0f )
    }));
}

TEST(MatrixTest, LookAtRHShouldCreateATransformThatSimulatesAStandardLookAtCamera_WithCenterAtTheMinus10ZLookingAtTheZAxisAndYOrientationShouldGenerateATranslationMatrix) {
    const auto lookAt1 = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
    EXPECT_EQ(lookAt1, XE::Matrix4f::translate(XE::Vector3f(0.0f, 0.0f, 10.0f)));

    const auto lookAt2 = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
    EXPECT_EQ(lookAt2, XE::Matrix4f::translate(XE::Vector3f(0.0f, 0.0f, 10.0f)));
}


TEST(MatrixTest, createPerspectiveShouldCreateAPerspectiveTransformationMatrix) {
    const auto m1 = XE::Matrix4f::perspective(XE::radians(60.0f), (320.0f/240.0f), 0.1f, 100.0f);
    EXPECT_EQ(m1, XE::Matrix4f::rows ({
        XE::Vector4f(1.299038170f, 0.000000000f, 0.000000000f, 0.000000000f),
        XE::Vector4f(0.000000000f, 1.73205090f, 0.000000000f, 0.000000000f),
        XE::Vector4f(0.000000000f, 0.000000000f, -1.002002001f, -0.200200200f),
        XE::Vector4f(0.000000000f, 0.000000000f, -1.000000000f, 0.000000000f)
    }));

    const auto m2 = XE::Matrix4f::perspective(XE::radians(120.0f), 1.33333f, 0.1f, 100.0f);
    EXPECT_EQ(m2, XE::Matrix4f::rows({
        XE::Vector4f(0.433013767f, 0.000000000f, 0.000000000f, 0.000000000f),
        XE::Vector4f(0.000000000f, 0.577350259f, 0.000000000f, 0.000000000f),
        XE::Vector4f(0.000000000f, 0.000000000f, -1.002002001f, -0.200200200f),
        XE::Vector4f(0.000000000f, 0.000000000f, -1.000000000f, 0.000000000f)
    }));
}


TEST(MatrixTest, CreateOrthographicShouldCreateAOrthographicTransformationMatrix) {
    const auto m1 = XE::Matrix4f::orthographic({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    EXPECT_EQ(m1, XE::Matrix4f::rows ({
        XE::Vector4f(1.0f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 1.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, -1.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
    }));

    const auto m2 = XE::Matrix4f::orthographic({-2.0f, -2.0f, -2.0f}, {2.0f, 2.0f, 2.0f});
    EXPECT_EQ(m2, XE::Matrix4f::rows ({
        XE::Vector4f(0.5f, 0.0f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.5f, 0.0f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, -0.5f, 0.0f),
        XE::Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
    }));

    const auto m3 = XE::Matrix4f::orthographic({-0.5f, -1.5f, 0.0f}, {2.5f, 3.5f, 100.0f});
    EXPECT_EQ(m3, XE::Matrix4f::rows ({
        XE::Vector4f(0.666666687f, 0.000000000f, 0.000000000f, -0.666666687f),
        XE::Vector4f(0.000000000f, 0.400000006f, 0.000000000f, -0.400000006f),
        XE::Vector4f(0.000000000f, 0.000000000f, -0.020000000f, -1.000000000f),
        XE::Vector4f(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f)
    }));

    const auto m4 = XE::Matrix4f::orthographic({-0.5f, -1.5f, 100.0f}, {2.5f, 3.5f, -50.0f});
    EXPECT_EQ(m4, XE::Matrix4f::rows({
        XE::Vector4f(0.666666687f, 0.000000000f, 0.000000000f, -0.666666687f),
        XE::Vector4f(0.000000000f, 0.400000006f, 0.000000000f, -0.400000006f),
        XE::Vector4f(0.000000000f, 0.000000000f, 0.013333334f, 0.333333343f),
        XE::Vector4f(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f)
    }));
}
