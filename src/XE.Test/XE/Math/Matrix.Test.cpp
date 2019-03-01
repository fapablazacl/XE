
#include <catch.hpp>

#include <XE/Math/Util.hpp>
#include <XE/Math/Matrix.hpp>

#include <iostream>

namespace Catch {
    template<>
    struct StringMaker<XE::Matrix2f> {
        static std::string convert(XE::Matrix2f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };

    template<>
    struct StringMaker<XE::Matrix3f> {
        static std::string convert(XE::Matrix3f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };

    template<>
    struct StringMaker<XE::Matrix4f> {
        static std::string convert(XE::Matrix4f const& value) {
            std::stringstream ss;
            ss << value;

            return ss.str();
        }
    };
}

using namespace XE;

TEST_CASE("Math::Matrix<3, float>") {
    SECTION("constructor should setup the matrix components correctly") {
        SECTION("for two-dimensional matrices") {      
            const Matrix2f m = {
                {1.0f, 2.0f}, 
                {3.0f, 4.0f}
            };

            REQUIRE(m.M11 == 1.0f); REQUIRE(m.M12 == 2.0f);
            REQUIRE(m.M21 == 3.0f); REQUIRE(m.M22 == 4.0f);

            REQUIRE(m.element[0][0] == 1.0f); REQUIRE(m.element[0][1] == 2.0f);
            REQUIRE(m.element[1][0] == 3.0f); REQUIRE(m.element[1][1] == 4.0f);

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

            REQUIRE(m.element[0][0] == 1.0f); REQUIRE(m.element[0][1] == 2.0f); REQUIRE(m.element[0][2] == 3.0f);
            REQUIRE(m.element[1][0] == 4.0f); REQUIRE(m.element[1][1] == 5.0f); REQUIRE(m.element[1][2] == 6.0f);
            REQUIRE(m.element[2][0] == 7.0f); REQUIRE(m.element[2][1] == 8.0f); REQUIRE(m.element[2][2] == 9.0f);

            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f);
            REQUIRE(m(1, 0) == 4.0f); REQUIRE(m(1, 1) == 5.0f); REQUIRE(m(1, 2) == 6.0f);
            REQUIRE(m(2, 0) == 7.0f); REQUIRE(m(2, 1) == 8.0f); REQUIRE(m(2, 2) == 9.0f);

            REQUIRE(m.data[0] == 1.0f); REQUIRE(m.data[1] == 2.0f); REQUIRE(m.data[2] == 3.0f);
            REQUIRE(m.data[3] == 4.0f); REQUIRE(m.data[4] == 5.0f); REQUIRE(m.data[5] == 6.0f);
            REQUIRE(m.data[6] == 7.0f); REQUIRE(m.data[7] == 8.0f); REQUIRE(m.data[8] == 9.0f);
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

            REQUIRE(m.element[0][0] == 1.0f); REQUIRE(m.element[0][1] == 2.0f); REQUIRE(m.element[0][2] == 3.0f); REQUIRE(m.element[0][3] == 4.0f);
            REQUIRE(m.element[1][0] == 5.0f); REQUIRE(m.element[1][1] == 6.0f); REQUIRE(m.element[1][2] == 7.0f); REQUIRE(m.element[1][3] == 8.0f);
            REQUIRE(m.element[2][0] == 9.0f); REQUIRE(m.element[2][1] == 10.0f); REQUIRE(m.element[2][2] == 11.0f); REQUIRE(m.element[2][3] == 12.0f);
            REQUIRE(m.element[3][0] == 13.0f); REQUIRE(m.element[3][1] == 14.0f); REQUIRE(m.element[3][2] == 15.0f); REQUIRE(m.element[3][3] == 16.0f);
            
            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f); REQUIRE(m(0, 3) == 4.0f);
            REQUIRE(m(1, 0) == 5.0f); REQUIRE(m(1, 1) == 6.0f); REQUIRE(m(1, 2) == 7.0f); REQUIRE(m(1, 3) == 8.0f);
            REQUIRE(m(2, 0) == 9.0f); REQUIRE(m(2, 1) == 10.0f); REQUIRE(m(2, 2) == 11.0f); REQUIRE(m(2, 3) == 12.0f);
            REQUIRE(m(3, 0) == 13.0f); REQUIRE(m(3, 1) == 14.0f); REQUIRE(m(3, 2) == 15.0f); REQUIRE(m(3, 3) == 16.0f);

            REQUIRE(m.data[0] == 1.0f); REQUIRE(m.data[1] == 2.0f); REQUIRE(m.data[2] == 3.0f); REQUIRE(m.data[3] == 4.0f);
            REQUIRE(m.data[4] == 5.0f); REQUIRE(m.data[5] == 6.0f); REQUIRE(m.data[6] == 7.0f); REQUIRE(m.data[7] == 8.0f);
            REQUIRE(m.data[8] == 9.0f); REQUIRE(m.data[9] == 10.0f); REQUIRE(m.data[10] == 11.0f); REQUIRE(m.data[11] == 12.0f);
            REQUIRE(m.data[12] == 13.0f); REQUIRE(m.data[13] == 14.0f); REQUIRE(m.data[14] == 15.0f); REQUIRE(m.data[15] == 16.0f);
        }

        SECTION("for four-dimensional matrices (single vector constructor)") {
            const Matrix4f m = {
                1.0f, 2.0f, 3.0f, 4.0f, 
                5.0f, 6.0f, 7.0f, 8.0f, 
                9.0f, 10.0f, 11.0f, 12.0f,
                13.0f, 14.0f, 15.0f, 16.0f
            };

            REQUIRE(m.M11 == 1.0f); REQUIRE(m.M12 == 2.0f); REQUIRE(m.M13 == 3.0f); REQUIRE(m.M14 == 4.0f);
            REQUIRE(m.M21 == 5.0f); REQUIRE(m.M22 == 6.0f); REQUIRE(m.M23 == 7.0f); REQUIRE(m.M24 == 8.0f);
            REQUIRE(m.M31 == 9.0f); REQUIRE(m.M32 == 10.0f); REQUIRE(m.M33 == 11.0f); REQUIRE(m.M34 == 12.0f);
            REQUIRE(m.M41 == 13.0f); REQUIRE(m.M42 == 14.0f); REQUIRE(m.M43 == 15.0f); REQUIRE(m.M44 == 16.0f);

            REQUIRE(m.element[0][0] == 1.0f); REQUIRE(m.element[0][1] == 2.0f); REQUIRE(m.element[0][2] == 3.0f); REQUIRE(m.element[0][3] == 4.0f);
            REQUIRE(m.element[1][0] == 5.0f); REQUIRE(m.element[1][1] == 6.0f); REQUIRE(m.element[1][2] == 7.0f); REQUIRE(m.element[1][3] == 8.0f);
            REQUIRE(m.element[2][0] == 9.0f); REQUIRE(m.element[2][1] == 10.0f); REQUIRE(m.element[2][2] == 11.0f); REQUIRE(m.element[2][3] == 12.0f);
            REQUIRE(m.element[3][0] == 13.0f); REQUIRE(m.element[3][1] == 14.0f); REQUIRE(m.element[3][2] == 15.0f); REQUIRE(m.element[3][3] == 16.0f);
            
            REQUIRE(m(0, 0) == 1.0f); REQUIRE(m(0, 1) == 2.0f); REQUIRE(m(0, 2) == 3.0f); REQUIRE(m(0, 3) == 4.0f);
            REQUIRE(m(1, 0) == 5.0f); REQUIRE(m(1, 1) == 6.0f); REQUIRE(m(1, 2) == 7.0f); REQUIRE(m(1, 3) == 8.0f);
            REQUIRE(m(2, 0) == 9.0f); REQUIRE(m(2, 1) == 10.0f); REQUIRE(m(2, 2) == 11.0f); REQUIRE(m(2, 3) == 12.0f);
            REQUIRE(m(3, 0) == 13.0f); REQUIRE(m(3, 1) == 14.0f); REQUIRE(m(3, 2) == 15.0f); REQUIRE(m(3, 3) == 16.0f);

            REQUIRE(m.data[0] == 1.0f); REQUIRE(m.data[1] == 2.0f); REQUIRE(m.data[2] == 3.0f); REQUIRE(m.data[3] == 4.0f);
            REQUIRE(m.data[4] == 5.0f); REQUIRE(m.data[5] == 6.0f); REQUIRE(m.data[6] == 7.0f); REQUIRE(m.data[7] == 8.0f);
            REQUIRE(m.data[8] == 9.0f); REQUIRE(m.data[9] == 10.0f); REQUIRE(m.data[10] == 11.0f); REQUIRE(m.data[11] == 12.0f);
            REQUIRE(m.data[12] == 13.0f); REQUIRE(m.data[13] == 14.0f); REQUIRE(m.data[14] == 15.0f); REQUIRE(m.data[15] == 16.0f);
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

    SECTION("getColumn should extract a certain column from the Matrix as a Vector") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.getColumn(0) == Vector4f{1.0f, 5.0f, 9.0f, 13.0f});
        REQUIRE(m.getColumn(1) == Vector4f{2.0f, 6.0f, 10.0f, 14.0f});
        REQUIRE(m.getColumn(2) == Vector4f{3.0f, 7.0f, 11.0f, 15.0f});
        REQUIRE(m.getColumn(3) == Vector4f{4.0f, 8.0f, 12.0f, 16.0f});
    }
    
    SECTION("setColumn should change correctly a certain column in the Matrix") {
        REQUIRE(Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        }.setColumn(0, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
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
        }.setColumn(1, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
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
        }.setColumn(2, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
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
        }.setColumn(3, Vector4f{4.0f, 3.0f, 2.0f, 1.0f}) == Matrix4f{
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 3.0f}, 
            {9.0f, 10.0f, 11.0f, 2.0f},
            {13.0f, 14.0f, 15.0f, 1.0f}
        });
    }

    SECTION("getRow should extract a certain row from the Matrix as a Vector") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.getRow(0) == Vector4f{1.0f, 2.0f, 3.0f, 4.0f});
        REQUIRE(m.getRow(1) == Vector4f{5.0f, 6.0f, 7.0f, 8.0f});
        REQUIRE(m.getRow(2) == Vector4f{9.0f, 10.0f, 11.0f, 12.0f});
        REQUIRE(m.getRow(3) == Vector4f{13.0f, 14.0f, 15.0f, 16.0f});
    }

    SECTION("getSubMatrix should extract a smaller matrix from another by discarding an entire row and column") {
        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(m.getSubMatrix(0, 0) == Matrix3f{
            {6.0f, 7.0f, 8.0f},
            {10.0f, 11.0f, 12.0f},
            {14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getSubMatrix(0, 1) == Matrix3f{
            {5.0f, 7.0f, 8.0f},
            {9.0f, 11.0f, 12.0f},
            {13.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getSubMatrix(1, 0) == Matrix3f{
            {2.0f, 3.0f, 4.0f},
            {10.0f, 11.0f, 12.0f},
            {14.0f, 15.0f, 16.0f}
        });

        REQUIRE(m.getSubMatrix(3, 3) == Matrix3f{
            {1.0f, 2.0f, 3.0f},
            {5.0f, 6.0f, 7.0f},
            {9.0f, 10.0f, 11.0f}
        });

        REQUIRE(m.getSubMatrix(3, 0) == Matrix3f{
            {2.0f, 3.0f, 4.0f},
            {6.0f, 7.0f, 8.0f},
            {10.0f, 11.0f, 12.0f},
        });
    }

    SECTION("Matrix-Vector multiply operation should transform the vector by the right") {
        const Matrix3f m = {
            {1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, -1.0f},
            {1.0f, 0.0f, 1.0f}
        };

        REQUIRE(m * Vector3f{0.0f, 0.0f, 0.0f} == Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(m * Vector3f{1.0f, 1.0f, 1.0f} == Vector3f{1.0f, -1.0f, 2.0f});
        REQUIRE(m * Vector3f{-1.0f, -1.0f, -1.0f} == Vector3f{-1.0f, 1.0f, -2.0f});
    }

    SECTION("Vector-Matrix multiply operation should transform the vector by the left") {
        const Matrix3f m = {
            {1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, -1.0f},
            {1.0f, 0.0f, 1.0f}
        };

        REQUIRE(Vector3f{0.0f, 0.0f, 0.0f} * m == Vector3f{0.0f, 0.0f, 0.0f});
        REQUIRE(Vector3f{1.0f, 1.0f, 1.0f} * m  == Vector3f{1.0f, 0.0f, 1.0f});
        REQUIRE(Vector3f{-1.0f, -1.0f, -1.0f} * m  == Vector3f{-1.0f, 0.0f, -1.0f});
    }

    SECTION("createZero static function should create a valid zero matrix") {
        REQUIRE(Matrix4f::createZero() == Matrix4f{
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f}
        });

        REQUIRE(Matrix3f::createZero() == Matrix3f{
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        });

        REQUIRE(Matrix2f::createZero() == Matrix2f{
            {0.0f, 0.0f},
            {0.0f, 0.0f}
        });

        const auto mzero = Matrix4f::createZero();

        REQUIRE(mzero * Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mzero * Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
    }

    SECTION("createIdentity static function should create a valid identity matrix") {
        REQUIRE(Matrix4f::createIdentity() == Matrix4f{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        });

        REQUIRE(Matrix3f::createIdentity() == Matrix3f{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        });

        REQUIRE(Matrix2f::createIdentity() == Matrix2f{
            {1.0f, 0.0f},
            {0.0f, 1.0f}
        });

        const auto mid = Matrix4f::createIdentity();
        REQUIRE(mid * Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mid * Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(mid * Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(mid * Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(mid * Vector4f{1.0f, 2.0f, -3.0f, 4.0f} == Vector4f{1.0f, 2.0f, -3.0f, 4.0f});
    }

    SECTION("createScaling static function should create a valid scaling matrix") {
        REQUIRE(Matrix4f::createScaling({1.0f, 2.0f, 3.0f, 4.0f}) == Matrix4f{
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 3.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 4.0f}
        });

        REQUIRE(Matrix3f::createScaling({1.0f, 2.0f, 3.0f}) == Matrix3f{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 2.0f, 0.0f},
            {0.0f, 0.0f, 3.0f},
        });

        REQUIRE(Matrix2f::createScaling({1.0f, 2.0f}) == Matrix2f{
            {1.0f, 0.0f},
            {0.0f, 2.0f},
        });

        const auto m_s1 = Matrix4f::createScaling({1.0f, 2.0f, 3.0f, 1.0f});
        REQUIRE(m_s1 * Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * Vector4f{1.0f, 0.0f, 0.0f, 0.0f} == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * Vector4f{0.0f, 1.0f, 0.0f, 0.0f} == Vector4f{0.0f, 2.0f, 0.0f, 0.0f});
        REQUIRE(m_s1 * Vector4f{0.0f, 0.0f, 1.0f, 0.0f} == Vector4f{0.0f, 0.0f, 3.0f, 0.0f});
        REQUIRE(m_s1 * Vector4f{1.0f, 2.0f, -3.0f, 4.0f} == Vector4f{1.0f, 4.0f, -9.0f, 4.0f});
    }
    
    SECTION("createTranslation static function should create a valid translate matrix") {
        const auto m1 = Matrix4f::createTranslation({2.0f, 3.0f, 4.0f, 1.0f});
        REQUIRE(m1.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 2.0f});
        REQUIRE(m1.getRow(1) == Vector4f{0.0f, 1.0f, 0.0f, 3.0f});
        REQUIRE(m1.getRow(2) == Vector4f{0.0f, 0.0f, 1.0f, 4.0f});
        REQUIRE(m1.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        const auto m2 = Matrix4f::createTranslation({2.0f, -3.0f, 4.0f});
        REQUIRE(m2.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 2.0f});
        REQUIRE(m2.getRow(1) == Vector4f{0.0f, 1.0f, 0.0f, -3.0f});
        REQUIRE(m2.getRow(2) == Vector4f{0.0f, 0.0f, 1.0f, 4.0f});
        REQUIRE(m2.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        REQUIRE(m1 * Vector4f{0.0f, 0.0f, 0.0f, 0.0f} == Vector4f{0.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m1 * Vector4f{2.0f, -2.0f, 1.0f, 1.0f} == Vector4f{4.0f, 1.0f, 5.0f, 1.0f});
        REQUIRE(m1 * Vector4f{1.0f, 0.0f, 1.0f, 1.0f} == Vector4f{3.0f, 3.0f, 5.0f, 1.0f});
        REQUIRE(m2 * Vector4f{1.0f, 1.0f, 1.0f, 1.0f} == Vector4f{3.0f, -2.0f, 5.0f, 1.0f});
        REQUIRE(m2 * Vector4f{0.0f, 0.0f, 0.0f, 1.0f} == Vector4f{2.0f, -3.0f, 4.0f, 1.0f});
    }
        
    SECTION("createRotationX static function should create a well-constructed rotation matrix") {
        Matrix4f m;
        float sin, cos;

        m = Matrix4f::createRotationX(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = Matrix4f::createRotationX(pi<float>);
        sin = std::sin(pi<float>);
        cos = std::cos(pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, cos, -sin, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f, sin, cos, 0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = Matrix4f::createRotationX(2.0f * pi<float>);
        sin = std::sin(2.0f * pi<float>);
        cos = std::cos(2.0f * pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, cos, -sin, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f, sin, cos, 0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
    }

    SECTION("createRotationY static function should create a well-constructed rotation matrix") {
        Matrix4f m;
        float sin, cos;

        m = Matrix4f::createRotationY(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = Matrix4f::createRotationY(pi<float>);
        sin = std::sin(pi<float>);
        cos = std::cos(pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{cos,  0.0f, sin,  0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, 1.0f,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{-sin,  0.0f,  cos,  0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f,  0.0f, 1.0f});

        m = Matrix4f::createRotationY(2.0f * pi<float>);
        sin = std::sin(2.0f * pi<float>);
        cos = std::cos(2.0f * pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{cos,  0.0f, sin,  0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, 1.0f,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{-sin,  0.0f,  cos,  0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f,  0.0f, 1.0f});
    }

    SECTION("createRotationZ static function should create a well-constructed rotation matrix") {
        Matrix4f m;
        float sin, cos;

        m = Matrix4f::createRotationZ(0.0f);
        sin = std::sin(0.0f);
        cos = std::cos(0.0f);
        REQUIRE(m.getRow(0) == Vector4f{1.0f, 0.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(1) == Vector4f{0.0f, 1.0f, 0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f, 0.0f, 1.0f, 0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f, 0.0f, 1.0f});

        m = Matrix4f::createRotationZ(pi<float>);
        sin = std::sin(pi<float>);
        cos = std::cos(pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{cos, -sin, 0.0f,  0.0f});
        REQUIRE(m.getRow(1) == Vector4f{sin, cos,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f,  0.0f,  1.0f,  0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f,  0.0f, 1.0f});

        m = Matrix4f::createRotationZ(2.0f * pi<float>);
        sin = std::sin(2.0f * pi<float>);
        cos = std::cos(2.0f * pi<float>);
        REQUIRE(m.getRow(0) == Vector4f{cos, -sin, 0.0f,  0.0f});
        REQUIRE(m.getRow(1) == Vector4f{sin, cos,  0.0f, 0.0f});
        REQUIRE(m.getRow(2) == Vector4f{0.0f,  0.0f,  1.0f,  0.0f});
        REQUIRE(m.getRow(3) == Vector4f{0.0f, 0.0f,  0.0f, 1.0f});
    }

    SECTION("createRotation with fixed axis should match the corresponding createRotation(X, Y Z) static methods") {
        REQUIRE(Matrix4f::createRotation(0.0f, {0.0f, 1.0f, 0.0f}) == Matrix4f::createIdentity());
        REQUIRE(Matrix4f::createRotation(0.0f, {0.0f, 0.0f, 1.0f}) == Matrix4f::createIdentity());
        REQUIRE(Matrix4f::createRotation(0.0f, {-1.0f, 0.0f, 0.0f}) == Matrix4f::createIdentity());
        REQUIRE(Matrix4f::createRotation(0.0f, {0.0f, -1.0f, 0.0f}) == Matrix4f::createIdentity());
        REQUIRE(Matrix4f::createRotation(0.0f, {0.0f, 0.0f, -1.0f}) == Matrix4f::createIdentity());
        
        REQUIRE(Matrix4f::createRotation(0.0f * pi<float>, {1.0f, 0.0f, 0.0f}) == Matrix4f::createRotationX(0.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(0.5f * pi<float>, {1.0f, 0.0f, 0.0f}) == Matrix4f::createRotationX(0.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.0f * pi<float>, {1.0f, 0.0f, 0.0f}) == Matrix4f::createRotationX(1.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.5f * pi<float>, {1.0f, 0.0f, 0.0f}) == Matrix4f::createRotationX(1.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(2.0f * pi<float>, {1.0f, 0.0f, 0.0f}) == Matrix4f::createRotationX(2.0f * pi<float>));

        REQUIRE(Matrix4f::createRotation(0.0f * pi<float>, {0.0f, 1.0f, 0.0f}) == Matrix4f::createRotationY(0.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(0.5f * pi<float>, {0.0f, 1.0f, 0.0f}) == Matrix4f::createRotationY(0.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.0f * pi<float>, {0.0f, 1.0f, 0.0f}) == Matrix4f::createRotationY(1.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.5f * pi<float>, {0.0f, 1.0f, 0.0f}) == Matrix4f::createRotationY(1.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(2.0f * pi<float>, {0.0f, 1.0f, 0.0f}) == Matrix4f::createRotationY(2.0f * pi<float>));
        
        REQUIRE(Matrix4f::createRotation(0.0f * pi<float>, {0.0f, 0.0f, 1.0f}) == Matrix4f::createRotationZ(0.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(0.5f * pi<float>, {0.0f, 0.0f, 1.0f}) == Matrix4f::createRotationZ(0.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.0f * pi<float>, {0.0f, 0.0f, 1.0f}) == Matrix4f::createRotationZ(1.0f * pi<float>));
        REQUIRE(Matrix4f::createRotation(1.5f * pi<float>, {0.0f, 0.0f, 1.0f}) == Matrix4f::createRotationZ(1.5f * pi<float>));
        REQUIRE(Matrix4f::createRotation(2.0f * pi<float>, {0.0f, 0.0f, 1.0f}) == Matrix4f::createRotationZ(2.0f * pi<float>));
    }

    SECTION("Determinant should compute the matrix determinant correctly") {
        const Matrix4f matA = {
            {1.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, 1.0f, -3.0f, -1.0f}, 
            {-3.0f, 2.0f, 1.0f, 0.0f}, 
            {2.0f, -1.0f, 0.0f, -1.0f}
        };

        // matrix determinant
        REQUIRE(abs(Matrix4f::createZero()) == 0.0f);
        REQUIRE(abs(Matrix4f::createIdentity()) == 1.0f);
        REQUIRE(abs(matA) == -32.0f);
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

        REQUIRE(matA + Matrix4f::createZero() == matA);
        REQUIRE(matB + Matrix4f::createZero() == matB);

        REQUIRE(matAddResult == matA + matB);
        REQUIRE(matAddResult == matB + matA);

        REQUIRE(matAddResult == ((+matA) += matB));
        REQUIRE(matAddResult == ((+matB) += matA));

        // subtraction
        REQUIRE(matNegA == -matA);
        REQUIRE(matA - Matrix4f::createZero() == matA);
        REQUIRE(matB - Matrix4f::createZero() == matB);

        REQUIRE(Matrix4f::createZero() - matA == -matA);
        REQUIRE(Matrix4f::createZero() - matB == -matB);

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
        REQUIRE(Matrix4f::createZero() == Matrix4f::createZero() * Matrix4f::createZero());
        REQUIRE(Matrix4f::createZero() == Matrix4f::createIdentity() * Matrix4f::createZero());
	    REQUIRE(Matrix4f::createIdentity() == Matrix4f::createIdentity() * Matrix4f::createIdentity());
        
	    REQUIRE(matA == matA * Matrix4f::createIdentity());
	    REQUIRE(matA == Matrix4f::createIdentity() * matA);

        REQUIRE(matMulResult == matA * matB);
        REQUIRE(matMulResult == ((+matA) *= matB));
    }

    SECTION("transpose should swap matrix rows and columns") {
        const auto mi = Matrix4f::createIdentity();
        const auto m0 = Matrix4f::createZero();

        REQUIRE(mi == transpose(mi));
        REQUIRE(m0 == transpose(m0));

        const Matrix4f m = {
            {1.0f, 2.0f, 3.0f, 4.0f}, 
            {5.0f, 6.0f, 7.0f, 8.0f}, 
            {9.0f, 10.0f, 11.0f, 12.0f},
            {13.0f, 14.0f, 15.0f, 16.0f}
        };

        REQUIRE(transpose(m) == Matrix4f{
            {1.0f, 5.0f, 9.0f, 13.0f}, 
            {2.0f, 6.0f, 10.0f,14.0f}, 
            {3.0f, 7.0f, 11.0f, 15.0f},
            {4.0f, 8.0f, 12.0f, 16.0f}
        });
    }

    SECTION("inverse should compute the matrix inverse multiplicative") {
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

        auto mi = Matrix4f::createIdentity();
        auto detMatA = -32.0f;

        REQUIRE(mi == inverse(mi));
        REQUIRE(invMatA == inverse(matA, detMatA));
        REQUIRE(invMatA == inverse(matA));
    }
}
