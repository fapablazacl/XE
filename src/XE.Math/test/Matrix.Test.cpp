
#include <catch2/catch_all.hpp>

#include <XE/Math/Common.h>
#include <XE/Math/Vector.h>
#include <XE/Math/Matrix.h>

TEST_CASE("Math::Matrix<3, float>", "[Matrix]") {
    SECTION("Determinant should compute the matrix determinant correctly") {
        const auto matA = XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 1.0f, 0.0f},
            XE::Vector4f{2.0f, 1.0f, -3.0f, -1.0f},
            XE::Vector4f{-3.0f, 2.0f, 1.0f, 0.0f},
            XE::Vector4f{2.0f, -1.0f, 0.0f, -1.0f}
        });

        // matrix determinant
        REQUIRE(XE::determinant(XE::Matrix4f::zero()) == 0.0f);
        REQUIRE(XE::determinant(XE::Matrix4f::identity()) == 1.0f);
        REQUIRE(XE::determinant(matA) == -32.0f);
    }

    SECTION("Arithmetic operators should behave according to their corresponding mathematical definitions") {
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
    
        /*const XE::Matrix4f matDivResult = XE::Matrix4f::rows({
            XE::Vector4f{-1.0f, 2.0f, 0.f,  0.0f},
            XE::Vector4f{ 2.0f, 0.0f, 3.0f, -1.0f},
            XE::Vector4f{-3.0f, 1.0f, 1.0f,  0.0f},
            XE::Vector4f{ 2.0f, 1.0f, 0.0f,  1.0f}
        });*/
    
        // addition
        REQUIRE(matA == +matA);
        REQUIRE(matB == +matB);

        REQUIRE(matA + XE::Matrix4f::zero() == matA);
        REQUIRE(matB + XE::Matrix4f::zero() == matB);

        REQUIRE(matAddResult == matA + matB);
        REQUIRE(matAddResult == matB + matA);

        REQUIRE(matAddResult == ((+matA) += matB));
        REQUIRE(matAddResult == ((+matB) += matA));

        // subtraction
        REQUIRE(matNegA == -matA);
        REQUIRE(matA - XE::Matrix4f::zero() == matA);
        REQUIRE(matB - XE::Matrix4f::zero() == matB);

        REQUIRE(XE::Matrix4f::zero() - matA == -matA);
        REQUIRE(XE::Matrix4f::zero() - matB == -matB);

        REQUIRE(matA - matB == matSubResult);
        REQUIRE(matB - matA == -matSubResult);
 
        REQUIRE(matSubResult == ((+matA) -= matB));
        REQUIRE(-matSubResult == ((+matB) -= matA));

        // scalar multiplication
        REQUIRE(matA * -1.0f == -matA);
        REQUIRE(matA * -1.0f == -1.0f * matA);
        REQUIRE(matA * 1.0f == matA);
        REQUIRE(matA * 1.0f == 1.0f * matA);

        // matrix multiplication
        REQUIRE(XE::Matrix4f::zero() == XE::Matrix4f::zero() * XE::Matrix4f::zero());
        REQUIRE(XE::Matrix4f::zero() == XE::Matrix4f::identity() * XE::Matrix4f::zero());
	    REQUIRE(XE::Matrix4f::identity() == XE::Matrix4f::identity() * XE::Matrix4f::identity());
        
	    REQUIRE(matA == matA * XE::Matrix4f::identity());
	    REQUIRE(matA == XE::Matrix4f::identity() * matA);

        REQUIRE(matMulResult == matA * matB);
        // REQUIRE(matMulResult == ((+matA) *= matB));
    }

    SECTION("transpose should swap matrix rows and columns") {
        const auto mi = XE::Matrix4f::identity();
        const auto m0 = XE::Matrix4f::zero();

        REQUIRE(mi == transpose(mi));
        REQUIRE(m0 == transpose(m0));

        const auto m = XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
        });

        REQUIRE(transpose(m) == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 5.0f, 9.0f, 13.0f},
            XE::Vector4f{2.0f, 6.0f, 10.0f,14.0f},
            XE::Vector4f{3.0f, 7.0f, 11.0f, 15.0f},
            XE::Vector4f{4.0f, 8.0f, 12.0f, 16.0f}
        }));
    }

    SECTION("inverse should compute the matrix inverse multiplicative") {
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

        REQUIRE(mi == inverse(mi));
        REQUIRE(invMatA == inverse(matA, detMatA));
        REQUIRE(invMatA == inverse(matA));
    }

    SECTION("constructor should setup the matrix components correctly") {
        SECTION("for two-dimensional matrices") {      
            const auto m = XE::Matrix2f::rows({
                XE::Vector2f{1.0f, 2.0f},
                XE::Vector2f{3.0f, 4.0f}
            });

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f);
            REQUIRE(m(1, 0) == 3.0f); REQUIRE(m(1, 1) == 4.0f);
        }

        SECTION("for three-dimensional matrices") {      
            const XE::Matrix3f m = XE::Matrix3f::rows({
                XE::Vector3f{1.0f, 2.0f, 3.0f},
                XE::Vector3f{4.0f, 5.0f, 6.0f},
                XE::Vector3f{7.0f, 8.0f, 9.0f}
            });

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f);
            REQUIRE(m(1, 0) == 4.0f); REQUIRE(m(1, 1) == 5.0f); REQUIRE(m(1, 2) == 6.0f);
            REQUIRE(m(2, 0) == 7.0f); REQUIRE(m(2, 1) == 8.0f); REQUIRE(m(2, 2) == 9.0f);
        }

        SECTION("for four-dimensional matrices") {
            const auto m = XE::Matrix4f::rows({
                XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
                XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
                XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
                XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
            });

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f); REQUIRE(m(0, 3) == 4.0f);
            REQUIRE(m(1, 0) == 5.0f); REQUIRE(m(1, 1) == 6.0f); REQUIRE(m(1, 2) == 7.0f); REQUIRE(m(1, 3) == 8.0f);
            REQUIRE(m(2, 0) == 9.0f); REQUIRE(m(2, 1) == 10.0f); REQUIRE(m(2, 2) == 11.0f); REQUIRE(m(2, 3) == 12.0f);
            REQUIRE(m(3, 0) == 13.0f); REQUIRE(m(3, 1) == 14.0f); REQUIRE(m(3, 2) == 15.0f); REQUIRE(m(3, 3) == 16.0f);
        }

        SECTION("for four-dimensional matrices (single vector constructor)") {
            const auto m = XE::Matrix4f::rows({
                XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
                XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
                XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
                XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
            });

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f); REQUIRE(m(0, 3) == 4.0f);
            REQUIRE(m(1, 0) == 5.0f); REQUIRE(m(1, 1) == 6.0f); REQUIRE(m(1, 2) == 7.0f); REQUIRE(m(1, 3) == 8.0f);
            REQUIRE(m(2, 0) == 9.0f); REQUIRE(m(2, 1) == 10.0f); REQUIRE(m(2, 2) == 11.0f); REQUIRE(m(2, 3) == 12.0f);
            REQUIRE(m(3, 0) == 13.0f); REQUIRE(m(3, 1) == 14.0f); REQUIRE(m(3, 2) == 15.0f); REQUIRE(m(3, 3) == 16.0f);
        }
    }

    SECTION("comparison operators should check matrix components") {
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

        REQUIRE(m1 == m1);
        REQUIRE(m2 == m2);
        REQUIRE(m1 != m2);
        REQUIRE(m2 != m1);
    }

    SECTION("getColumn should extract a certain column from the Matrix as a Vector") {
        const XE::Matrix4f m = XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getColumn(0) == XE::Vector4f{1.0f, 5.0f, 9.0f, 13.0f});
        REQUIRE(m.getColumn(1) == XE::Vector4f{2.0f, 6.0f, 10.0f, 14.0f});
        REQUIRE(m.getColumn(2) == XE::Vector4f{3.0f, 7.0f, 11.0f, 15.0f});
        REQUIRE(m.getColumn(3) == XE::Vector4f{4.0f, 8.0f, 12.0f, 16.0f});
    }
    
    SECTION("setColumn should change correctly a certain column in the Matrix") {
        REQUIRE(
            XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}}).setColumn(0, XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) ==
            XE::Matrix4f::rows({
            XE::Vector4f{4.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{3.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{2.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{1.0f, 14.0f, 15.0f, 16.0f}}));

        REQUIRE(XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}})
        .setColumn(1, XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 4.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 3.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 2.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 1.0f, 15.0f, 16.0f}}));

        REQUIRE(XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
        }).setColumn(2, XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 4.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 3.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 2.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 1.0f, 16.0f}
        }));
        
        REQUIRE(XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}})
        .setColumn(3, XE::Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 3.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 2.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 1.0f}
        }));
    }

    SECTION("getRow should extract a certain row from the Matrix as a Vector") {
        const XE::Matrix4f m = XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f});
    }

    SECTION("getSubMatrix should extract a smaller matrix from another by discarding an entire row and column") {
        const XE::Matrix4f m = XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 2.0f, 3.0f, 4.0f},
            XE::Vector4f{5.0f, 6.0f, 7.0f, 8.0f},
            XE::Vector4f{9.0f, 10.0f, 11.0f, 12.0f},
            XE::Vector4f{13.0f, 14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getSubMatrix(0, 0) == XE::Matrix3f::rows({
            XE::Vector3f{6.0f, 7.0f, 8.0f},
            XE::Vector3f{10.0f, 11.0f, 12.0f},
            XE::Vector3f{14.0f, 15.0f, 16.0f}
        }));

        REQUIRE(m.getSubMatrix(0, 1) == XE::Matrix3f::rows({
            XE::Vector3f{5.0f, 7.0f, 8.0f},
            XE::Vector3f{9.0f, 11.0f, 12.0f},
            XE::Vector3f{13.0f, 15.0f, 16.0f}
        }));

        REQUIRE(m.getSubMatrix(1, 0) == XE::Matrix3f::rows({
            XE::Vector3f{2.0f, 3.0f, 4.0f},
            XE::Vector3f{10.0f, 11.0f, 12.0f},
            XE::Vector3f{14.0f, 15.0f, 16.0f}
        }));

        REQUIRE(m.getSubMatrix(3, 3) == XE::Matrix3f::rows({
            XE::Vector3f{1.0f, 2.0f, 3.0f},
            XE::Vector3f{5.0f, 6.0f, 7.0f},
            XE::Vector3f{9.0f, 10.0f, 11.0f}
        }));

        REQUIRE(m.getSubMatrix(3, 0) == XE::Matrix3f::rows({
            XE::Vector3f{2.0f, 3.0f, 4.0f},
            XE::Vector3f{6.0f, 7.0f, 8.0f},
            XE::Vector3f{10.0f, 11.0f, 12.0f},
        }));
    }

    SECTION("Matrix-Vector multiply operation should transform the vector by the right") {
        const XE::Matrix3f m = XE::Matrix3f::rows({
            XE::Vector3f{1.0f, -1.0f, 1.0f},
            XE::Vector3f{-1.0f, 1.0f, -1.0f},
            XE::Vector3f{1.0f, 0.0f, 1.0f}
        });

        REQUIRE(m * XE::Vector3f{0.0f, 0.0f, 0.0f} == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(m * XE::Vector3f{1.0f, 1.0f, 1.0f} == XE::Vector3f{1.0f, -1.0f, 2.0f});
        REQUIRE(m * XE::Vector3f{-1.0f, -1.0f, -1.0f} == XE::Vector3f{-1.0f, 1.0f, -2.0f});
    }

    SECTION("Vector-Matrix multiply operation should transform the vector by the left") {
        const auto m = XE::Matrix3f::rows({
            XE::Vector3f{1.0f, -1.0f, 1.0f},
            XE::Vector3f{-1.0f, 1.0f, -1.0f},
            XE::Vector3f{1.0f, 0.0f, 1.0f}
        });

        REQUIRE(XE::Vector3f{0.0f, 0.0f, 0.0f} * m == XE::Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(XE::Vector3f{1.0f, 1.0f, 1.0f} * m  == XE::Vector3f{1.0f, 0.0f, 1.0f});
        REQUIRE(XE::Vector3f{-1.0f, -1.0f, -1.0f} * m  == XE::Vector3f{-1.0f, 0.0f, -1.0f});
    }

    SECTION("zero static function should create a valid zero matrix") {
        REQUIRE(XE::Matrix4f::zero() == XE::Matrix4f::rows({
            XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f}
        }));

        REQUIRE(XE::Matrix3f::zero() == XE::Matrix3f::rows({
            XE::Vector3f{0.0f, 0.0f, 0.0f},
            XE::Vector3f{0.0f, 0.0f, 0.0f},
            XE::Vector3f{0.0f, 0.0f, 0.0f}
        }));

        REQUIRE(XE::Matrix2f::zero() == XE::Matrix2f::rows({
            XE::Vector2f{0.0f, 0.0f},
            XE::Vector2f{0.0f, 0.0f}}
        ));

        const auto mzero = XE::Matrix4f::zero();

        REQUIRE(mzero * XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
    }

    SECTION("identity static function should create a valid identity matrix") {
        REQUIRE(XE::Matrix4f::identity() == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f}
        }));

        REQUIRE(XE::Matrix3f::identity() == XE::Matrix3f::rows({
            XE::Vector3f{1.0f, 0.0f, 0.0f},
            XE::Vector3f{0.0f, 1.0f, 0.0f},
            XE::Vector3f{0.0f, 0.0f, 1.0f}}
        ));

        REQUIRE(XE::Matrix2f::identity() == XE::Matrix2f::rows({
            XE::Vector2f{1.0f, 0.0f},
            XE::Vector2f{0.0f, 1.0f}}
        ));

        const auto mid = XE::Matrix4f::identity();
        REQUIRE(mid * XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mid * XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mid * XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(mid * XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(mid * XE::Vector4f{1.0f, 2.0f, -3.0f, 4.0f} == XE::Vector4f{1.0f, 2.0f, -3.0f, 4.0f});
    }

    SECTION("scale static function should create a valid scaling matrix") {
        REQUIRE(XE::Matrix4f::scale({1.0f, 2.0f, 3.0f, 4.0f}) == XE::Matrix4f::rows({
            XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 2.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 3.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 4.0f}
        }));

        REQUIRE(XE::Matrix3f::scale({1.0f, 2.0f, 3.0f}) == XE::Matrix3f::rows({
            XE::Vector3f{1.0f, 0.0f, 0.0f},
            XE::Vector3f{0.0f, 2.0f, 0.0f},
            XE::Vector3f{0.0f, 0.0f, 3.0f}}
        ));

        REQUIRE(XE::Matrix2f::scale({1.0f, 2.0f}) == XE::Matrix2f::rows({
            XE::Vector2f{1.0f, 0.0f},
            XE::Vector2f{0.0f, 2.0f}}
        ));

        const auto m_s1 = XE::Matrix4f::scale({1.0f, 2.0f, 3.0f, 1.0f});
        REQUIRE(m_s1 * XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == XE::Vector4f{0.0f, 2.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == XE::Vector4f{0.0f, 0.0f, 3.0f, 0.0f});
        REQUIRE(m_s1 * XE::Vector4f{1.0f, 2.0f, -3.0f, 4.0f} == XE::Vector4f{1.0f, 4.0f, -9.0f, 4.0f});
    }
    
    SECTION("createTranslation static function should create a valid translate matrix") {
        const auto m1 = XE::Matrix4f::translate({2.0f, 3.0f, 4.0f, 1.0f});
        REQUIRE(m1.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m1.getRow(1) == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m1.getRow(2) == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m1.getRow(3) == XE::Vector4f{2.0f, 3.0f, 4.0f, 1.0f});

        const auto m2 = XE::Matrix4f::translate({2.0f, -3.0f, 4.0f});
        REQUIRE(m2.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m2.getRow(1) == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m2.getRow(2) == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m2.getRow(3) == XE::Vector4f{2.0f, -3.0f, 4.0f, 1.0f});

        REQUIRE(XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f} * m1 == XE::Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(XE::Vector4f{2.0f, -2.0f, 1.0f, 1.0f} * m1 == XE::Vector4f{4.0f, 1.0f, 5.0f, 1.0f});
        REQUIRE(XE::Vector4f{1.0f, 0.0f, 1.0f, 1.0f} * m1 == XE::Vector4f{3.0f, 3.0f, 5.0f, 1.0f});
        REQUIRE(XE::Vector4f{1.0f, 1.0f, 1.0f, 1.0f} * m2 == XE::Vector4f{3.0f, -2.0f, 5.0f, 1.0f});
        REQUIRE(XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f} * m2 == XE::Vector4f{2.0f, -3.0f, 4.0f, 1.0f});

        REQUIRE(XE::Vector4f{2.0f, -2.0f, 1.0f, 0.0f} * m1 == XE::Vector4f{2.0f, -2.0f, 1.0f, 0.0f});
        REQUIRE(XE::Vector4f{1.0f, 0.0f, 1.0f, 0.0f} * m1 == XE::Vector4f{1.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(XE::Vector4f{1.0f, 1.0f, 1.0f, 0.0f} * m2 == XE::Vector4f{1.0f, 1.0f, 1.0f, 0.0f});
    }
        
    SECTION("rotateX static function should create a well-constructed rotation matrix") {
        XE::Matrix4f m;
        float sin, cos;

        m = XE::Matrix4f::rotateX(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = XE::Matrix4f::rotateX(XE::pi<float>);
        sin = std::sin(XE::pi<float>);
        cos = std::cos(XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, cos, -sin, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f, sin, cos, 0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = XE::Matrix4f::rotateX(2.0f * XE::pi<float>);
        sin = std::sin(2.0f * XE::pi<float>);
        cos = std::cos(2.0f * XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, cos, -sin, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f, sin, cos, 0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
    }

    SECTION("rotateY static function should create a well-constructed rotation matrix") {
        XE::Matrix4f m;
        float sin, cos;

        m = XE::Matrix4f::rotateY(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = XE::Matrix4f::rotateY(XE::pi<float>);
        sin = std::sin(XE::pi<float>);
        cos = std::cos(XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{cos,  0.0f, sin,  0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, 1.0f,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{-sin,  0.0f,  cos,  0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f,  0.0f, 1.0f});

        m = XE::Matrix4f::rotateY(2.0f * XE::pi<float>);
        sin = std::sin(2.0f * XE::pi<float>);
        cos = std::cos(2.0f * XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{cos,  0.0f, sin,  0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, 1.0f,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{-sin,  0.0f,  cos,  0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f,  0.0f, 1.0f});
    }

    SECTION("rotateZ static function should create a well-constructed rotation matrix") {
        XE::Matrix4f m;
        float sin, cos;

        m = XE::Matrix4f::rotateZ(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = XE::Matrix4f::rotateZ(XE::pi<float>);
        sin = std::sin(XE::pi<float>);
        cos = std::cos(XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{cos, -sin, 0.0f,  0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{sin, cos,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f,  0.0f,  1.0f,  0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f,  0.0f, 1.0f});

        m = XE::Matrix4f::rotateZ(2.0f * XE::pi<float>);
        sin = std::sin(2.0f * XE::pi<float>);
        cos = std::cos(2.0f * XE::pi<float>);
        REQUIRE(m.getRow(0) == XE::Vector4f{cos, -sin, 0.0f,  0.0f});
        REQUIRE(m.getRow(1) == XE::Vector4f{sin, cos,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == XE::Vector4f{0.0f,  0.0f,  1.0f,  0.0f});
        REQUIRE(m.getRow(3) == XE::Vector4f{0.0f, 0.0f,  0.0f, 1.0f});
    }

    SECTION("rotate with fixed axis should match the corresponding rotate(X, Y Z) static methods") {
        REQUIRE(XE::Matrix4f::rotate(0.0f, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::identity());
        REQUIRE(XE::Matrix4f::rotate(0.0f, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::identity());
        REQUIRE(XE::Matrix4f::rotate(0.0f, {-1.0f, 0.0f, 0.0f}) == XE::Matrix4f::identity());
        REQUIRE(XE::Matrix4f::rotate(0.0f, {0.0f, -1.0f, 0.0f}) == XE::Matrix4f::identity());
        REQUIRE(XE::Matrix4f::rotate(0.0f, {0.0f, 0.0f, -1.0f}) == XE::Matrix4f::identity());
        
        REQUIRE(XE::Matrix4f::rotate(0.0f * XE::pi<float>, {1.0f, 0.0f, 0.0f}) == XE::Matrix4f::rotateX(0.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(0.5f * XE::pi<float>, {1.0f, 0.0f, 0.0f}) == XE::Matrix4f::rotateX(0.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.0f * XE::pi<float>, {1.0f, 0.0f, 0.0f}) == XE::Matrix4f::rotateX(1.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.5f * XE::pi<float>, {1.0f, 0.0f, 0.0f}) == XE::Matrix4f::rotateX(1.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(2.0f * XE::pi<float>, {1.0f, 0.0f, 0.0f}) == XE::Matrix4f::rotateX(2.0f * XE::pi<float>));

        REQUIRE(XE::Matrix4f::rotate(0.0f * XE::pi<float>, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::rotateY(0.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(0.5f * XE::pi<float>, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::rotateY(0.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.0f * XE::pi<float>, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::rotateY(1.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.5f * XE::pi<float>, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::rotateY(1.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(2.0f * XE::pi<float>, {0.0f, 1.0f, 0.0f}) == XE::Matrix4f::rotateY(2.0f * XE::pi<float>));
        
        REQUIRE(XE::Matrix4f::rotate(0.0f * XE::pi<float>, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::rotateZ(0.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(0.5f * XE::pi<float>, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::rotateZ(0.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.0f * XE::pi<float>, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::rotateZ(1.0f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(1.5f * XE::pi<float>, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::rotateZ(1.5f * XE::pi<float>));
        REQUIRE(XE::Matrix4f::rotate(2.0f * XE::pi<float>, {0.0f, 0.0f, 1.0f}) == XE::Matrix4f::rotateZ(2.0f * XE::pi<float>));
    }

    
    SECTION("createLookAt should create a transform matrix that simulates a standard look-at camera") {
        SECTION("with center at the origin, looking at the -Z Axis, and +Y orientation, should generate an identity matrix") {
            const auto lookAt = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
            REQUIRE(lookAt == XE::Matrix4f::identity());
        }
        
        SECTION("with center at the origin, looking at the +Z Axis, and +Y orientation, should generate an pseudo identity matrix with some negative unit axis") {
            const auto lookAt = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
            
            REQUIRE(lookAt == XE::Matrix4f::rows({
                XE::Vector4f{ -1.0f, 0.0f, 0.0f, 0.0f },
                XE::Vector4f{ 0.0f, 1.0f, 0.0f, 0.0f },
                XE::Vector4f{ 0.0f, 0.0f, -1.0f, 0.0f },
                XE::Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f }
            }));
        }
        
        SECTION("with center at the -10 Z, looking at the +Z Axis, and +Y orientation, should generate a translation matrix") {
            const auto lookAt1 = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});
            REQUIRE(lookAt1 == XE::Matrix4f::translate({0.0f, 0.0f, -10.0f}));
            
            const auto lookAt2 = XE::Matrix4f::lookAtRH({0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f});
            REQUIRE(lookAt2 == XE::Matrix4f::translate({0.0f, 0.0f, -10.0f}));
        }
    }

    
//    SECTION("createPerspective should create a perspective transformation matrix") {
//        const auto m1 = XE::Matrix4f::perspective(XE::radians(60.0f), (320.0f/240.0f), 0.1f, 100.0f);
//        REQUIRE(m1 == XE::Matrix4f::rows ({
//            XE::Vector4f{1.299038170f, 0.000000000f, 0.000000000f, 0.000000000f},
//            XE::Vector4f{0.000000000f, 1.73205090f, 0.000000000f, 0.000000000f},
//            XE::Vector4f{0.000000000f, 0.000000000f, -1.002002001f, -0.200200200f},
//            XE::Vector4f{0.000000000f, 0.000000000f, -1.000000000f, 1.000000000f}
//        }));
//
//        const auto m2 = XE::Matrix4f::perspective(XE::radians(120.0f), 1.33333f, 0.1f, 100.0f);
//        REQUIRE(m2 == XE::Matrix4f::rows({
//            XE::Vector4f{0.433013767f, 0.000000000f, 0.000000000f, 0.000000000f},
//            XE::Vector4f{0.000000000f, 0.577350259f, 0.000000000f, 0.000000000f},
//            XE::Vector4f{0.000000000f, 0.000000000f, -1.002002001f, -0.200200200f},
//            XE::Vector4f{0.000000000f, 0.000000000f, -1.000000000f, 1.000000000f}
//        }));
//    }

    SECTION("createOrthographic should create a orthographic transformation matrix") {
        const auto m1 = XE::Matrix4f::orthographic({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
        REQUIRE(m1 == XE::Matrix4f::rows ({
            XE::Vector4f{1.0f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 1.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, -1.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f},
        }));

        const auto m2 = XE::Matrix4f::orthographic({-2.0f, -2.0f, -2.0f}, {2.0f, 2.0f, 2.0f});
        REQUIRE(m2 == XE::Matrix4f::rows ({
            XE::Vector4f{0.5f, 0.0f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.5f, 0.0f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, -0.5f, 0.0f},
            XE::Vector4f{0.0f, 0.0f, 0.0f, 1.0f},
        }));

        const auto m3 = XE::Matrix4f::orthographic({-0.5f, -1.5f, 0.0f}, {2.5f, 3.5f, 100.0f});
        REQUIRE(m3 == XE::Matrix4f::rows ({
            XE::Vector4f{0.666666687f, 0.000000000f, 0.000000000f, -0.666666687f},
            XE::Vector4f{0.000000000f, 0.400000006f, 0.000000000f, -0.400000006f},
            XE::Vector4f{0.000000000f, 0.000000000f, -0.020000000f, -1.000000000f},
            XE::Vector4f{0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f}
        }));

        const auto m4 = XE::Matrix4f::orthographic({-0.5f, -1.5f, 100.0f}, {2.5f, 3.5f, -50.0f});
        REQUIRE(m4 == XE::Matrix4f::rows({
            XE::Vector4f{0.666666687f, 0.000000000f, 0.000000000f, -0.666666687f},
            XE::Vector4f{0.000000000f, 0.400000006f, 0.000000000f, -0.400000006f},
            XE::Vector4f{0.000000000f, 0.000000000f, 0.013333334f, 0.333333343f},
            XE::Vector4f{0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f}
        }));
    }
}
