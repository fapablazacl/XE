
#include <catch.hpp>
#include <XE/Math/Matrix.hpp>

namespace XE {
    template<typename T, int R, int C>
    std::string ToString(const XE::Math::Matrix<T, R, C> &m) {
        return "ToString(const XE::Math::Matrix<T, R, C> &m)";
    }
}

namespace Catch {
    template<>
    struct StringMaker<XE::Math::Matrix3f> {
        static std::string convert(XE::Math::Matrix3f const& value) {
            return XE::ToString(value);
        }
    };

    template<>
    struct StringMaker<XE::Math::Matrix4f> {
        static std::string convert(XE::Math::Matrix4f const& value) {
            return XE::ToString(value);
        }
    };
}

using namespace XE;
using namespace Math;

TEST_CASE("Math::Vector<3, float>") {
    const Matrix4f matA = {
        {1.0f, 2.0f, 1.0f, 0.0f}, 
        {2.0f, 1.0f, -3.0f, -1.0f}, 
        {-3.0f, 2.0f, 1.0f, 0.0f}, 
        {2.0f, -1.0f, 0.0f, -1.0f}
    };
    
    const Matrix4f matB = {
        {-3.0f, 1.0f, 5.0f, 1.0f},
        {1.0f, 2.0f, -1.0f, 1.0f},
        {1.0f, 2.0f, 1.0f, -2.0f},
        {1.0f, -1.0f, -3.0f, -1.0f}
    };
    
    const auto subIdentity = Matrix3f::Identity();
    const auto identity = Matrix4f::Identity();
    const auto zero = Matrix3f::Zero();
    
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
    
    const Matrix4f invMatA = {
        {0.25000f,  0.000f, -0.25000f,  0.000},
        {0.28125f,  0.125f,  0.09375f, -0.125},
        {0.18750f, -0.250f,  0.06250f,  0.250},
        {0.21875f, -0.125f, -0.59375f, -0.875}
    };
 
    const float detA = -32.0f;
    
    Matrix4f aux;
    
    // matrix element retrieval by function paramenters
    REQUIRE(matA(0, 0) == 1.0f);
    REQUIRE(matA(0, 1) == 2.0f);
    REQUIRE(matA(0, 2) == 1.0f);
    REQUIRE(matA(0, 3) == 0.0f);
        
    REQUIRE(matA(1, 0) == 2.0f);
    REQUIRE(matA(1, 1) == 1.0f);
    REQUIRE(matA(1, 2) == -3.0f);
    REQUIRE(matA(1, 3) == -1.0f);
        
    REQUIRE(matA(2, 0) == -3.0f);
    REQUIRE(matA(2, 1) == 2.0f);
    REQUIRE(matA(2, 2) == 1.0f);
    REQUIRE(matA(2, 3) == 0.0f);
        
    REQUIRE(matA(3, 0) == 2.0f);
    REQUIRE(matA(3, 1) == -1.0f);
    REQUIRE(matA(3, 2) == 0.0f);
    REQUIRE(matA(3, 3) == -1.0f);
        
    // matrix element retrieval by template paramenters
    // matrix row vectors
    REQUIRE(matA.GetRowVector(0) == Vector4f(1.0f, 2.0f, 1.0f, 0.0f));
    REQUIRE(matA.GetRowVector(1) == Vector4f(2.0f, 1.0f, -3.0f, -1.0f));
    REQUIRE(matA.GetRowVector(2) == Vector4f(-3.0f, 2.0f, 1.0f, 0.0f));
    REQUIRE(matA.GetRowVector(3) == Vector4f(2.0f, -1.0f, 0.0f, -1.0f));
        
    // matrix column vectors
    REQUIRE(matA.GetColumnVector(0) == Vector4f(1.0f, 2.0f, -3.0f, 2.0f));
    REQUIRE(matA.GetColumnVector(1) == Vector4f(2.0f, 1.0f, 2.0f, -1.0f));
    REQUIRE(matA.GetColumnVector(2) == Vector4f(1.0f, -3.0f, 1.0f, 0.0f));
    REQUIRE(matA.GetColumnVector(3) == Vector4f(0.0f, -1.0f, 0.0f, -1.0f));
        
    // matrix direct comparison
    REQUIRE(matA != matB);
    
    // matrix scale
    REQUIRE(matA * -1.0f == -matA);
    REQUIRE(matA * -1.0f == -1.0f * matA);
    REQUIRE(matA * 1.0f == matA);
        
    // matrix adition 
    REQUIRE(matAddResult == matA + matB);
    REQUIRE(matSubResult == matA - matB);
        
    aux = matA; aux += matB;
    REQUIRE(matAddResult == aux);
        
    aux = matA; aux -= matB;
    REQUIRE(matSubResult == aux);
    
    // submatrix
    REQUIRE(subIdentity == identity.SubMatrix(0, 0));
        
    // matrix determinant
    REQUIRE( Abs(identity) == 1.0f );
    REQUIRE( Abs(matA) == detA );
        
    //matrix multiply
    REQUIRE(matMulResult == matA * matB);
    REQUIRE(matMulResult == ((aux = matA) *= matB));
    
	REQUIRE(identity == identity * identity);

	REQUIRE(matA == matA * identity);
	REQUIRE(matA == identity * matA);

    //inverse matrix
    aux = Inverse(matA);

	REQUIRE(invMatA == aux);

	// vector transformation
    /*
	Matrix4f translation = Matrix4f::Translate({1.0f, 1.0f, 1.0f, 1.0f});
	Vector3f position1 = {0.0f, -1.0f, 0.0f};
	Vector3f position2_1 = {1.0f,  0.0f, 1.0f};
	Vector3f position2_2 = transform<float, 4>(translation, position1);
	
	REQUIRE(position2_1, position2_2);
    */
}
