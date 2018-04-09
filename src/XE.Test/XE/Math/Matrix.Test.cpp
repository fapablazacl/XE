
#include <catch.hpp>
#include <XE/Math/Matrix.hpp>

namespace Catch {
    template<>
    struct StringMaker<XE::Math::Matrix2f> {
        static std::string convert(XE::Math::Matrix2f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };

    template<>
    struct StringMaker<XE::Math::Matrix3f> {
        static std::string convert(XE::Math::Matrix3f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };

    template<>
    struct StringMaker<XE::Math::Matrix4f> {
        static std::string convert(XE::Math::Matrix4f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };
}

using namespace XE;
using namespace Math;

TEST_CASE("Math::Matrix<3, float>") {
    SECTION("constructor should setup the matrix components correctly") {
        SECTION("for two-dimensional matrices") {      
            const Matrix2f m = {
                {1.0f, 2.0f}, 
                {3.0f, 4.0f}
            };

            REQUIRE(m.M11 == 1.0f); REQUIRE(m.M12 == 2.0f);
            REQUIRE(m.M21 == 3.0f); REQUIRE(m.M22 == 4.0f);

            REQUIRE(m.Element[0][0] == 1.0f); REQUIRE(m.Element[0][1] == 2.0f);
            REQUIRE(m.Element[1][0] == 3.0f); REQUIRE(m.Element[1][1] == 4.0f);

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f);
            REQUIRE(m(1, 0) == 3.0f); REQUIRE(m(1, 1) == 4.0f);
        }

        SECTION("for three-dimensional matrices") {      
            const Matrix3f m = {
                {1.0f, 2.0f, 3.0f}, 
                {4.0f, 5.0f, 6.0f}, 
                {7.0f, 8.0f, 9.0f}
            };

            REQUIRE(m.M11 == 1.0f); REQUIRE(m.M12 == 2.0f); REQUIRE(m.M13 == 3.0f);
            REQUIRE(m.M21 == 4.0f); REQUIRE(m.M22 == 5.0f); REQUIRE(m.M23 == 6.0f);
            REQUIRE(m.M31 == 7.0f); REQUIRE(m.M32 == 8.0f); REQUIRE(m.M33 == 9.0f);

            REQUIRE(m.Element[0][0] == 1.0f); REQUIRE(m.Element[0][1] == 2.0f); REQUIRE(m.Element[0][2] == 3.0f);
            REQUIRE(m.Element[1][0] == 4.0f); REQUIRE(m.Element[1][1] == 5.0f); REQUIRE(m.Element[1][2] == 6.0f);
            REQUIRE(m.Element[2][0] == 7.0f); REQUIRE(m.Element[2][1] == 8.0f); REQUIRE(m.Element[2][2] == 9.0f);

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f);
            REQUIRE(m(1, 0) == 4.0f); REQUIRE(m(1, 1) == 5.0f); REQUIRE(m(1, 2) == 6.0f);
            REQUIRE(m(2, 0) == 7.0f); REQUIRE(m(2, 1) == 8.0f); REQUIRE(m(2, 2) == 9.0f);
        }

        SECTION("for four-dimensional matrices") {
            const Matrix4f m = {
                {1.0f, 2.0f, 3.0f, 4.0f}, 
                {5.0f, 6.0f, 7.0f, 8.0f}, 
                {9.0f, 10.0f, 11.0f, 12.0f},
                {13.0f, 14.0f, 15.0f, 16.0f}
            };

            REQUIRE(m.M11 == 1.0f); REQUIRE(m.M12 == 2.0f); REQUIRE(m.M13 == 3.0f); REQUIRE(m.M14 == 4.0f);
            REQUIRE(m.M21 == 5.0f); REQUIRE(m.M22 == 6.0f); REQUIRE(m.M23 == 7.0f); REQUIRE(m.M24 == 8.0f);
            REQUIRE(m.M31 == 9.0f); REQUIRE(m.M32 == 10.0f); REQUIRE(m.M33 == 11.0f); REQUIRE(m.M34 == 12.0f);
            REQUIRE(m.M41 == 13.0f); REQUIRE(m.M42 == 14.0f); REQUIRE(m.M43 == 15.0f); REQUIRE(m.M44 == 16.0f);

            REQUIRE(m.Element[0][0] == 1.0f); REQUIRE(m.Element[0][1] == 2.0f); REQUIRE(m.Element[0][2] == 3.0f); REQUIRE(m.Element[0][3] == 4.0f);
            REQUIRE(m.Element[1][0] == 5.0f); REQUIRE(m.Element[1][1] == 6.0f); REQUIRE(m.Element[1][2] == 7.0f); REQUIRE(m.Element[1][3] == 8.0f);
            REQUIRE(m.Element[2][0] == 9.0f); REQUIRE(m.Element[2][1] == 10.0f); REQUIRE(m.Element[2][2] == 11.0f); REQUIRE(m.Element[2][3] == 12.0f);
            REQUIRE(m.Element[3][0] == 13.0f); REQUIRE(m.Element[3][1] == 14.0f); REQUIRE(m.Element[3][2] == 15.0f); REQUIRE(m.Element[3][3] == 16.0f);
            
            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f); REQUIRE(m(0, 3) == 4.0f);
            REQUIRE(m(1, 0) == 5.0f); REQUIRE(m(1, 1) == 6.0f); REQUIRE(m(1, 2) == 7.0f); REQUIRE(m(1, 3) == 8.0f);
            REQUIRE(m(2, 0) == 9.0f); REQUIRE(m(2, 1) == 10.0f); REQUIRE(m(2, 2) == 11.0f); REQUIRE(m(2, 3) == 12.0f);
            REQUIRE(m(3, 0) == 13.0f); REQUIRE(m(3, 1) == 14.0f); REQUIRE(m(3, 2) == 15.0f); REQUIRE(m(3, 3) == 16.0f);
        }
    }

    SECTION("comparison operators should check matrix components") {
        const Matrix4f m1 = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        const Matrix4f m2 = {
            {16.0f, 15.0f, 14.0f, 13.0f}, 
            {12.0f, 11.0f, 10.0f, 9.0f}, 
            {8.0f, 7.0f, 6.0f, 5.0f},
            {4.0f, 3.0f, 2.0f, 1.0f}
        };

        REQUIRE(m1 == m1);
        REQUIRE(m2 == m2);
        REQUIRE(m1 != m2);
        REQUIRE(m2 != m1);
    }

    SECTION("GetColumnVector should extract a certain column from the Matrix as a Vector") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.GetColumnVector(0) == Vector4f{1.0f, 5.0f, 9.0f, 13.0f});
        REQUIRE(m.GetColumnVector(1) == Vector4f{2.0f, 6.0f, 10.0f, 14.0f});
        REQUIRE(m.GetColumnVector(2) == Vector4f{3.0f, 7.0f, 11.0f, 15.0f});
        REQUIRE(m.GetColumnVector(3) == Vector4f{4.0f, 8.0f, 12.0f, 16.0f});
    }
    
    SECTION("SetColumnVector should change correctly a certain column in the Matrix") {
        REQUIRE(Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        }.SetColumnVector(0, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
            {4.0f, 2.0f, 3.0f, 4.0f}, 
            {3.0f, 6.0f, 7.0f, 8.0f}, 
            {2.0f, 10.0f, 11.0f, 12.0f},
            {1.0f, 14.0f, 15.0f, 16.0f}
        });

        REQUIRE(Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        }.SetColumnVector(1, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
            {1.0f, 4.0f, 3.0f, 4.0f}, 
            {5.0f, 3.0f, 7.0f, 8.0f}, 
            {9.0f, 2.0f, 11.0f, 12.0f},
            {13.0f, 1.0f, 15.0f, 16.0f}
        });

        REQUIRE(Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        }.SetColumnVector(2, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
            {1.0f, 2.0f, 4.0f, 4.0f}, 
            {5.0f, 6.0f, 3.0f, 8.0f}, 
            {9.0f, 10.0f, 2.0f, 12.0f},
            {13.0f, 14.0f, 1.0f, 16.0f}
        });
        
        REQUIRE(Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        }.SetColumnVector(3, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 3.0f}, 
            {9.0f, 10.0f, 11.0f, 2.0f},
            {13.0f, 14.0f, 15.0f, 1.0f}
        });
    }

    SECTION("GetRowVector should extract a certain row from the Matrix as a Vector") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.GetRowVector(0) == Vector4f{1.0f, 2.0f, 3.0f, 4.0f});
        REQUIRE(m.GetRowVector(1) == Vector4f{5.0f, 6.0f, 7.0f, 8.0f});
        REQUIRE(m.GetRowVector(2) == Vector4f{9.0f, 10.0f, 11.0f, 12.0f});
        REQUIRE(m.GetRowVector(3) == Vector4f{13.0f, 14.0f, 15.0f, 16.0f});
    }

    SECTION("SubMatrix should extract a smaller matrix from another by discarding an entire row and column") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.SubMatrix(0, 0) == Matrix3f{
            {6.0f, 7.0f, 8.0f},
            {10.0f, 11.0f, 12.0f},
            {14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.SubMatrix(0, 1) == Matrix3f{
            {5.0f, 7.0f, 8.0f},
            {9.0f, 11.0f, 12.0f},
            {13.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.SubMatrix(1, 0) == Matrix3f{
            {2.0f, 3.0f, 4.0f},
            {10.0f, 11.0f, 12.0f},
            {14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.SubMatrix(3, 3) == Matrix3f{
            {1.0f, 2.0f, 3.0f},
            {5.0f, 6.0f, 7.0f},
            {9.0f, 10.0f, 11.0f}
        });

        REQUIRE(m.SubMatrix(3, 0) == Matrix3f{
            {2.0f, 3.0f, 4.0f},
            {6.0f, 7.0f, 8.0f},
            {10.0f, 11.0f, 12.0f},
        });
    }

    SECTION("Zero static function should create a valid zero matrix") {
        REQUIRE(Matrix4f::Zero() == Matrix4f{
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f}
        });

        REQUIRE(Matrix3f::Zero() == Matrix3f{
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        });

        REQUIRE(Matrix2f::Zero() == Matrix2f{
            {0.0f, 0.0f},
            {0.0f, 0.0f}
        });
    }

    SECTION("Identity static function should create a valid identity matrix") {
        REQUIRE(Matrix4f::Identity() == Matrix4f{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        });

        REQUIRE(Matrix3f::Identity() == Matrix3f{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        });

        REQUIRE(Matrix2f::Identity() == Matrix2f{
            {1.0f, 0.0f},
            {0.0f, 1.0f}
        });
    }

    SECTION("Determinant should compute the matrix determinant correctly") {
        const Matrix4f matA = {
            {1.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, 1.0f, -3.0f, -1.0f}, 
            {-3.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, -1.0f, 0.0f, -1.0f}
        };

        // matrix determinant
        REQUIRE(Abs(Matrix4f::Zero()) == 0.0f);
        REQUIRE(Abs(Matrix4f::Identity()) == 1.0f);
        REQUIRE(Abs(matA) == -32.0f);
    }

    SECTION("Arithmetic operators should behave according to their corresponding mathematical definitions") {
        const Matrix4f matA = {
            {1.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, 1.0f, -3.0f, -1.0f}, 
            {-3.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, -1.0f, 0.0f, -1.0f}
        };
    
        const Matrix4f matNegA = {
            {-1.0f, -2.0f, -1.0f, -0.0f}, 
            {-2.0f, -1.0f, 3.0f, 1.0f}, 
            {3.0f, -2.0f, -1.0f, -0.0f}, 
            {-2.0f, 1.0f, -0.0f, 1.0f}
        };

        const Matrix4f matB = {
            {-3.0f, 1.0f, 5.0f, 1.0f},
            {1.0f, 2.0f, -1.0f, 1.0f},
            {1.0f, 2.0f, 1.0f, -2.0f},
            {1.0f, -1.0f, -3.0f, -1.0f}
        };

        const Matrix4f matAddResult = {
            {-2.0f,  3.0f,  6.0f,  1.0f},
            { 3.0f,  3.0f, -4.0f,  0.0f},
            {-2.0f,  4.0f,  2.0f, -2.0f},
            { 3.0f, -2.0f, -3.0f, -2.0f}
        };
    
        const Matrix4f matSubResult = {
            { 4.0f,  1.0f, -4.0f, -1.0f},
            { 1.0f, -1.0f, -2.0f, -2.0f},
            {-4.0f,  0.0f,  0.0f,  2.0f},
            { 1.0f,  0.0f,  3.0f,  0.0f}
        };
    
        const Matrix4f matMulResult = {
            {  0.0f,  7.0f,  4.0f,   1.0f},
            { -9.0f, -1.0f,  9.0f,  10.0f},
            { 12.0f,  3.0f, -16.0f, -3.0f},
            { -8.0f,  1.0f,  14.0f,  2.0f}
        };
    
        const Matrix4f matDivResult = {
            {-1.0f, 2.0f, 0.f,  0.0f},
            { 2.0f, 0.0f, 3.0f, -1.0f},
            {-3.0f, 1.0f, 1.0f,  0.0f},
            { 2.0f, 1.0f, 0.0f,  1.0f}
        };
    
        // addition
        REQUIRE(matA == +matA);
        REQUIRE(matB == +matB);

        REQUIRE(matA + Matrix4f::Zero() == matA);
        REQUIRE(matB + Matrix4f::Zero() == matB);

        REQUIRE(matAddResult == matA + matB);
        REQUIRE(matAddResult == matB + matA);

        REQUIRE(matAddResult == ((+matA) += matB));
        REQUIRE(matAddResult == ((+matB) += matA));

        // subtraction
        REQUIRE(matNegA == -matA);
        REQUIRE(matA - Matrix4f::Zero() == matA);
        REQUIRE(matB - Matrix4f::Zero() == matB);

        REQUIRE(Matrix4f::Zero() - matA == -matA);
        REQUIRE(Matrix4f::Zero() - matB == -matB);

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
        REQUIRE(Matrix4f::Zero() == Matrix4f::Zero() * Matrix4f::Zero());
        REQUIRE(Matrix4f::Zero() == Matrix4f::Identity() * Matrix4f::Zero());
	    REQUIRE(Matrix4f::Identity() == Matrix4f::Identity() * Matrix4f::Identity());
        
	    REQUIRE(matA == matA * Matrix4f::Identity());
	    REQUIRE(matA == Matrix4f::Identity() * matA);

        REQUIRE(matMulResult == matA * matB);
        REQUIRE(matMulResult == ((+matA) *= matB));
    }

    SECTION("Transpose should swap matrix rows and columns") {
        auto mi = Matrix4f::Identity();
        auto m0 = Matrix4f::Zero();

        REQUIRE(mi == Transpose(mi));
        REQUIRE(m0 == Transpose(m0));

        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(Transpose(m) == Matrix4f{
            {1.0f, 5.0f, 9.0f, 13.0f}, 
            {2.0f, 6.0f, 10.0f,14.0f}, 
            {3.0f, 7.0f, 11.0f, 15.0f},
            {4.0f, 8.0f, 12.0f, 16.0f}
        });
    }

    SECTION("Inverse should swap matrix rows and columns") {        
        const Matrix4f invMatA = {
            {0.25000f,  0.000f, -0.25000f,  0.000},
            {0.28125f,  0.125f,  0.09375f, -0.125},
            {0.18750f, -0.250f,  0.06250f,  0.250},
            {0.21875f, -0.125f, -0.59375f, -0.875}
        };

        const Matrix4f matA = {
            {1.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, 1.0f, -3.0f, -1.0f}, 
            {-3.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, -1.0f, 0.0f, -1.0f}
        };

        auto mi = Matrix4f::Identity();
        auto detMatA = -32.0f;

        REQUIRE(mi == Inverse(mi));
        REQUIRE(invMatA == Inverse(matA, detMatA));
        REQUIRE(invMatA == Inverse(matA));
    }

    /*
	// vector transformation
	Matrix4f translation = Matrix4f::Translate({1.0f, 1.0f, 1.0f, 1.0f});
	Vector3f position1 = {0.0f, -1.0f, 0.0f};
	Vector3f position2_1 = {1.0f,  0.0f, 1.0f};
	Vector3f position2_2 = transform<float, 4>(translation, position1);
	
	REQUIRE(position2_1, position2_2);
    */
}
