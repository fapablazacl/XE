#include <catch2/catch_test_macros.hpp>

#include "xe/math/Range.h"

TEST_CASE("trange default constructor is [0, 1]", "[range]") {
    const xe::trange<float> r;
    REQUIRE(r.min == 0.0f);
    REQUIRE(r.max == 1.0f);
    REQUIRE(r.max >= r.min);
}

TEST_CASE("trange two-param constructor normalizes min/max", "[range]") {
    const xe::trange<float> r{1.0f, -1.0f};
    REQUIRE(r.min == -1.0f);
    REQUIRE(r.max == 1.0f);
}

TEST_CASE("expand leaves the range alone when value is already inside", "[range][expand]") {
    xe::trange<float> r;
    for (float value : {0.0f, 0.25f, 0.5f, 0.75f, 1.0f}) {
        r.expand(value);
        REQUIRE(r.min == 0.0f);
        REQUIRE(r.max == 1.0f);
    }
}

TEST_CASE("expand grows the range when value is outside", "[range][expand]") {
    xe::trange<float> r;

    r.expand(-1.0f);
    REQUIRE(r.min == -1.0f);
    REQUIRE(r.max == 1.0f);

    r.expand(2.0f);
    REQUIRE(r.min == -1.0f);
    REQUIRE(r.max == 2.0f);

    r.expand(-2.0f);
    REQUIRE(r.min == -2.0f);
    REQUIRE(r.max == 2.0f);
}

TEST_CASE("partialOverlap returns true for overlapping ranges", "[range][overlap]") {
    const xe::trange<float> subject;
    const xe::trange<float> cases[] = {
        {0.5f, 1.5f},
        {-1.5f, 0.5f},
        {0.25f, 1.5f},
        {-1.5f, 0.75f},
        {-10.5f, 0.1f},
        {-1.5f, 2.0f},
    };
    for (const auto &r : cases) {
        REQUIRE(subject.partialOverlap(r));
    }
}

TEST_CASE("overlap returns true in all overlap cases", "[range][overlap]") {
    const xe::trange<float> subject;
    const xe::trange<float> cases[] = {
        {0.5f, 1.5f},
        {-1.5f, 0.5f},
        {0.25f, 1.5f},
        {-1.5f, 0.75f},
        {-10.5f, 0.1f},
        {-1.5f, 2.0f},
        {-1.0f, 1.0f},
        {0.0f, 1.0f},
        {-2.0f, 2.0f},
    };
    for (const auto &r : cases) {
        REQUIRE(subject.overlap(r));
    }
}

TEST_CASE("tightly abutting ranges do not partially overlap", "[range][overlap]") {
    for (int i = -10; i < 10; ++i) {
        for (float offset = 0.0f; offset < 1.0f; offset += 0.125f) {
            const float width = 1.0f;
            const xe::trange<float> a{(i + 0) * width + offset, (i + 1) * width + offset};
            const xe::trange<float> b{(i + 1) * width + offset, (i + 2) * width + offset};
            REQUIRE_FALSE(a.partialOverlap(b));
        }
    }
}

TEST_CASE("legacy Range alias still works", "[range][legacy]") {
    const xe::Range r;
    REQUIRE(r.max == 1.0f);
}
