
#include "xe/math/Plane.h"
#include "GoogleTestCommon.h"

TEST(PlaneTest, DefaultConstructorMakesXZPlanePointingPlusYAxis) {
    XE::Plane subject;

    EXPECT_EQ(subject.a, 0.0f);
    EXPECT_EQ(subject.b, 1.0f);
    EXPECT_EQ(subject.c, 0.0f);
    EXPECT_EQ(subject.d, 0.0f);
}

TEST(PlaneTest, FourParamConstructorMakesAPlaneWithSpecificEquationTerms) {
    XE::Plane subject{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_EQ(subject.a, 1.0f);
    EXPECT_EQ(subject.b, 2.0f);
    EXPECT_EQ(subject.c, 3.0f);
    EXPECT_EQ(subject.d, 4.0f);
}

TEST(PlaneTest, ConstDataRetunsAPointerToInternalArray) {
    const XE::Plane subject{1.0f, 2.0f, 3.0f, 4.0f};

    const float *values = subject.data();

    EXPECT_EQ(values[0], subject.a);
    EXPECT_EQ(values[1], subject.b);
    EXPECT_EQ(values[2], subject.c);
    EXPECT_EQ(values[3], subject.d);

    EXPECT_EQ(values[0], 1.0f);
    EXPECT_EQ(values[1], 2.0f);
    EXPECT_EQ(values[2], 3.0f);
    EXPECT_EQ(values[3], 4.0f);
}

TEST(PlaneTest, DataRetunsAPointerToInternalArray) {
    XE::Plane subject{1.0f, 2.0f, 3.0f, 4.0f};

    const float *values = subject.data();

    EXPECT_EQ(values[0], subject.a);
    EXPECT_EQ(values[1], subject.b);
    EXPECT_EQ(values[2], subject.c);
    EXPECT_EQ(values[3], subject.d);

    EXPECT_EQ(values[0], 1.0f);
    EXPECT_EQ(values[1], 2.0f);
    EXPECT_EQ(values[2], 3.0f);
    EXPECT_EQ(values[3], 4.0f);
}

struct TestCase {
    XE::Plane input{};
    bool output{false};
};

TEST(PlaneTest, EqualityOperatorChecksForEqualityForEachTerm) {
    XE::Plane subject{1.0f, 2.0f, 3.0f, 4.0f};

    const TestCase testCases[] = {TestCase{XE::Plane{1.0f, 2.0f, 3.0f, 4.0f}, true}, TestCase{XE::Plane{-1.0f, 2.0f, 3.0f, 4.0f}, false},
                                  TestCase{XE::Plane{1.0f, -2.0f, 3.0f, 4.0f}, false}, TestCase{XE::Plane{1.0f, 2.0f, -3.0f, 4.0f}, false},
                                  TestCase{XE::Plane{1.0f, 2.0f, 3.0f, -4.0f}, false}};

    for (const auto &testCase : testCases) {
        EXPECT_EQ(testCase.output, subject.operator==(testCase.input));
    }
}

TEST(PlaneTest, InequalityOperatorChecksForEqualityForEachTerm) {
    XE::Plane subject{1.0f, 2.0f, 3.0f, 4.0f};

    const TestCase testCases[] = {TestCase{XE::Plane{1.0f, 2.0f, 3.0f, 4.0f}, false}, TestCase{XE::Plane{-1.0f, 2.0f, 3.0f, 4.0f}, true},
                                  TestCase{XE::Plane{1.0f, -2.0f, 3.0f, 4.0f}, true}, TestCase{XE::Plane{1.0f, 2.0f, -3.0f, 4.0f}, true},
                                  TestCase{XE::Plane{1.0f, 2.0f, 3.0f, -4.0f}, true}};

    for (const auto &testCase : testCases) {
        EXPECT_EQ(testCase.output, subject.operator!=(testCase.input));
    }
}

TEST(PlaneTest, VectorialFactoryMethodGeneratesAPlaneFromASpecificPointAndANormalVector) {
    struct VectorialInput {
        XE::Vector3 normal;
        XE::Vector3 position;
    };

    struct VectorialTestCase {
        VectorialInput input;
        XE::Plane output;
    };

    const VectorialTestCase testCases[] = {
        {VectorialInput{XE::Vector3{1.0f, 0.0f, 0.0f}, XE::Vector3{0.0f, 0.0f, 0.0f}}, XE::Plane{1.0f, 0.0f, 0.0f, 0.0f}},
        {VectorialInput{XE::Vector3{0.0f, 1.0f, 0.0f}, XE::Vector3{0.0f, 0.0f, 0.0f}}, XE::Plane{0.0f, 1.0f, 0.0f, 0.0f}},
        {VectorialInput{XE::Vector3{0.0f, 0.0f, 1.0f}, XE::Vector3{0.0f, 0.0f, 0.0f}}, XE::Plane{0.0f, 0.0f, 1.0f, 0.0f}},
        {VectorialInput{XE::Vector3{1.0f, 0.0f, 0.0f}, XE::Vector3{1.0f, 1.0f, 1.0f}}, XE::Plane{1.0f, 0.0f, 0.0f, 1.0f}},
        {VectorialInput{XE::Vector3{0.0f, 1.0f, 0.0f}, XE::Vector3{1.0f, 1.0f, 1.0f}}, XE::Plane{0.0f, 1.0f, 0.0f, 1.0f}},
        {VectorialInput{XE::Vector3{0.0f, 0.0f, 1.0f}, XE::Vector3{1.0f, 1.0f, 1.0f}}, XE::Plane{0.0f, 0.0f, 1.0f, 1.0f}},
    };

    for (const auto &testCase : testCases) {
        const auto &input = testCase.input;
        const auto output = XE::planeVectorial(input.normal, input.position);

        EXPECT_EQ(testCase.output, output);
        EXPECT_EQ(XE::norm2(output.normal()), 1.0f);
    }
}


TEST(PlaneTest, EvaluateReturnsTheResultScalarOfEvaluatingPointAgainstThePlaneEquation) {
    using XE::Plane;

    Plane plane{-1.0f, 2.0f, -3.0f, 1.0f};

    EXPECT_EQ(plane.evaluate({0.0f, 0.0f, 0.0f}), -1.0f);
    EXPECT_EQ(plane.evaluate({1.0f, 1.0f, 1.0f}), -3.0f);
    EXPECT_EQ(plane.evaluate({-1.0f, 0.0f, 0.0f}), 0.0f);
    EXPECT_EQ(plane.evaluate({-1.0f, 1.0f, 0.0f}), 2.0f);
}



TEST(PlaneTest, IntersectChecksIfTwoPlanesIntersect) {
    using XE::Plane;

    Plane plane1{1.0f, 0.0f, 0.0f, 1.0f};
    Plane plane2{0.0f, 1.0f, 0.0f, 1.0f};
    Plane plane3{0.0f, 0.0f, 1.0f, 1.0f};

    EXPECT_TRUE(plane1.intersect(plane2));
    EXPECT_TRUE(plane2.intersect(plane3));
    EXPECT_TRUE(plane3.intersect(plane1));
}


TEST(PlaneTest, TestChecksHowAPointRelatesToAPlane) {
    using XE::Plane;
    using XE::PlaneSide;

    Plane plane{0.0f, 1.0f, 0.0f, 1.0f};

    EXPECT_EQ(plane.test({0.0f, 2.0f, 0.0f}), PlaneSide::Front);
    EXPECT_EQ(plane.test({0.0f, -2.0f, 0.0f}), PlaneSide::Back);
    EXPECT_EQ(plane.test({0.0f, 1.0f, 0.0f}), PlaneSide::Inside);
}


TEST(PlaneTest, SerializationGeneratesANonEmptyString) {
    XE::Plane subject;

    std::stringstream ss;
    ss << subject;

    EXPECT_NE(ss.str(), "");
}

TEST(PlaneTest, SerializationGeneratesDiffrentStringsForDifferentPlanes) {
    std::stringstream ss1;
    ss1 << XE::planeXY();

    std::stringstream ss2;
    ss2 << XE::planeYZ();

    EXPECT_NE(ss1.str(), ss2.str());
}
