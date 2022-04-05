
#include <XE/Math/Plane.h>
#include "Common.h"

TEST_CASE("Plane constructors should create planes with specific values", "[Plane]") {
    SECTION("default constructor generate a XZ plane, pointing to the positive Y-axis") {
        XE::Planef subject;

        REQUIRE(subject.a == 0.0f);
        REQUIRE(subject.b == 1.0f);
        REQUIRE(subject.c == 0.0f);
        REQUIRE(subject.d == 0.0f);
    }

    SECTION("4-param constructor generate a plane with the specified components") {
        XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

        REQUIRE(subject.a == 1.0f);
        REQUIRE(subject.b == 2.0f);
        REQUIRE(subject.c == 3.0f);
        REQUIRE(subject.d == 4.0f);
    }

    SECTION("const data() must return a pointer to an 4 elements array.") {
        const XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

        const float* values = subject.data();

        REQUIRE(values[0] == subject.a);
        REQUIRE(values[1] == subject.b);
        REQUIRE(values[2] == subject.c);
        REQUIRE(values[3] == subject.d);

        REQUIRE(values[0] == 1.0f);
        REQUIRE(values[1] == 2.0f);
        REQUIRE(values[2] == 3.0f);
        REQUIRE(values[3] == 4.0f);
    }

    SECTION("data() must return a pointer to an 4 elements array.") {
        XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

        const float* values = subject.data();

        REQUIRE(values[0] == subject.a);
        REQUIRE(values[1] == subject.b);
        REQUIRE(values[2] == subject.c);
        REQUIRE(values[3] == subject.d);

        REQUIRE(values[0] == 1.0f);
        REQUIRE(values[1] == 2.0f);
        REQUIRE(values[2] == 3.0f);
        REQUIRE(values[3] == 4.0f);
    }
}

TEST_CASE("Plane comparison operators must compare planes using each component in the equation", "[Plane]") {
    XE::Planef subject{1.0f, 2.0f, 3.0f, 4.0f};

    struct TestCase {
        XE::Planef input;
        bool output;
    };

    SECTION("operator==() check for equality for all components in the Plane") {
        const TestCase testCases[] = {
            TestCase{XE::Planef{1.0f, 2.0f, 3.0f, 4.0f}, true}, 
            TestCase{XE::Planef{-1.0f, 2.0f, 3.0f, 4.0f}, false}, 
            TestCase{XE::Planef{1.0f, -2.0f, 3.0f, 4.0f}, false}, 
            TestCase{XE::Planef{1.0f, 2.0f, -3.0f, 4.0f}, false}, 
            TestCase{XE::Planef{1.0f, 2.0f, 3.0f, -4.0f}, false}
        };

        for (const auto &testCase : testCases) {
            REQUIRE(testCase.output == subject.operator==(testCase.input));
        }
    }

    SECTION("operator!=() check for inequality for all components in the Plane") {
        const TestCase testCases[] = {
            TestCase{XE::Planef{1.0f, 2.0f, 3.0f, 4.0f}, false}, 
            TestCase{XE::Planef{-1.0f, 2.0f, 3.0f, 4.0f}, true}, 
            TestCase{XE::Planef{1.0f, -2.0f, 3.0f, 4.0f}, true}, 
            TestCase{XE::Planef{1.0f, 2.0f, -3.0f, 4.0f}, true}, 
            TestCase{XE::Planef{1.0f, 2.0f, 3.0f, -4.0f}, true}
        };

        for (const auto &testCase : testCases) {
            REQUIRE(testCase.output == subject.operator!=(testCase.input));
        }
    }
}

TEST_CASE("Plane factory methods should create well-defined planes with normalized normals. All the resulting normals have one-length (normalized)", "[Plane]") {
    SECTION("vectorial() must create a scalar plane form from a vectorial plane form (with a normal and a position).") {
        struct VectorialInput {
            XE::Vector3f normal;
            XE::Vector3f position;
        };

        struct VectorialTestCase {
            VectorialInput input;
            XE::Planef output;
        };

        const VectorialTestCase testCases[] = {
            {
                VectorialInput{XE::Vector3f{1.0f, 0.0f, 0.0f}, XE::Vector3f{0.0f, 0.0f, 0.0f}},
                XE::Planef{1.0f, 0.0f, 0.0f, 0.0f}
            },
            {
                VectorialInput{XE::Vector3f{0.0f, 1.0f, 0.0f}, XE::Vector3f{0.0f, 0.0f, 0.0f}},
                XE::Planef{0.0f, 1.0f, 0.0f, 0.0f}
            },
            {
                VectorialInput{XE::Vector3f{0.0f, 0.0f, 1.0f}, XE::Vector3f{0.0f, 0.0f, 0.0f}},
                XE::Planef{0.0f, 0.0f, 1.0f, 0.0f}
            },
            {
                VectorialInput{XE::Vector3f{1.0f, 0.0f, 0.0f}, XE::Vector3f{1.0f, 1.0f, 1.0f}},
                XE::Planef{1.0f, 0.0f, 0.0f, 1.0f}
            },
            {
                VectorialInput{XE::Vector3f{0.0f, 1.0f, 0.0f}, XE::Vector3f{1.0f, 1.0f, 1.0f}},
                XE::Planef{0.0f, 1.0f, 0.0f, 1.0f}
            },
            {
                VectorialInput{XE::Vector3f{0.0f, 0.0f, 1.0f}, XE::Vector3f{1.0f, 1.0f, 1.0f}},
                XE::Planef{0.0f, 0.0f, 1.0f, 1.0f}
            },
        };

        for (const auto &testCase : testCases) {
            const auto &input = testCase.input;
            const auto output = XE::Planef::vectorial(input.normal, input.position);

            REQUIRE(testCase.output == output);
            REQUIRE(XE::norm2(output.normal()) == 1.0f);
        }
    }

    SECTION("xy() must create a plane with a normal pointing to the Z-axis") {
        
    }
}

TEST_CASE("Plane can be Serialized to an Output Stream", "[Plane]") {
    SECTION("serialization should generate a non-empty string") {
        XE::Planef subject;

        std::stringstream ss;
        ss << subject;

        REQUIRE(ss.str() != "");
    }

    SECTION("serialization of different value objects should generate different non-empty string") {
        std::stringstream ss1;
        ss1 << XE::Planef::xy();

        std::stringstream ss2;
        ss2 << XE::Planef::yz();

        REQUIRE(ss1.str() != ss2.str());
    }
}
