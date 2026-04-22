/**
 * @file Vector.Xcheck.Test.cpp
 * @brief Cross-check xe::vec3 / xe::vec4 operations against glm on random inputs.
 *
 * Uses a fixed-seed std::mt19937 (see MathTestHelpers.h) so any failing
 * sample is reproducible across machines. Two ranges per op — [-1000, 1000]
 * to hit the general case and [-1, 1] to expose precision issues at small
 * magnitudes.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <glm/glm.hpp>

#include "MathTestHelpers.h"
#include "xe/math/Vector.h"

using xe::test::randomVec3;
using xe::test::randomVec4;
using xe::test::requireNear;
using xe::test::requireVecNear;
using xe::test::seededRng;
using xe::test::toGlm;

namespace {
    constexpr int kSamples = 64;
} // namespace

TEST_CASE("vec3 arithmetic matches glm on random inputs", "[vector][xcheck]") {
    auto rng = seededRng();
    const auto lo = GENERATE(-1000.0f, -1.0f);
    const float hi = -lo;
    CAPTURE(lo, hi);

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 a = randomVec3(rng, lo, hi);
        const xe::vec3 b = randomVec3(rng, lo, hi);
        const glm::vec3 ga = toGlm(a);
        const glm::vec3 gb = toGlm(b);

        requireVecNear(a + b, ga + gb);
        requireVecNear(a - b, ga - gb);
        requireVecNear(a * b, ga * gb);
        requireVecNear(-a, -ga);
    }
}

TEST_CASE("vec3 scalar ops match glm on random inputs", "[vector][xcheck]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> sd(-10.0f, 10.0f);

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 v = randomVec3(rng, -100.0f, 100.0f);
        const float s = sd(rng);
        const glm::vec3 gv = toGlm(v);

        requireVecNear(v * s, gv * s);
        requireVecNear(s * v, s * gv);
    }
}

TEST_CASE("vec3 dot/cross/length match glm on random inputs", "[vector][xcheck]") {
    auto rng = seededRng();
    const auto lo = GENERATE(-1000.0f, -1.0f);
    const float hi = -lo;
    CAPTURE(lo, hi);

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 a = randomVec3(rng, lo, hi);
        const xe::vec3 b = randomVec3(rng, lo, hi);
        const glm::vec3 ga = toGlm(a);
        const glm::vec3 gb = toGlm(b);

        requireNear(xe::dot(a, b), glm::dot(ga, gb));
        requireVecNear(xe::cross(a, b), glm::cross(ga, gb));
        requireNear(xe::length(a), glm::length(ga));
        requireNear(xe::length2(a), glm::dot(ga, ga));
    }
}

TEST_CASE("vec3 normalize matches glm on random inputs", "[vector][xcheck]") {
    auto rng = seededRng();

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        // Keep magnitude comfortably away from zero so normalize is well-defined.
        const xe::vec3 v = randomVec3(rng, -100.0f, 100.0f);
        if (xe::length2(v) < 1e-6f) {
            continue;
        }
        requireVecNear(xe::normalize(v), glm::normalize(toGlm(v)));
    }
}

TEST_CASE("vec3 min/max match glm on random inputs", "[vector][xcheck]") {
    auto rng = seededRng();

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 a = randomVec3(rng, -1000.0f, 1000.0f);
        const xe::vec3 b = randomVec3(rng, -1000.0f, 1000.0f);
        requireVecNear(xe::min(a, b), glm::min(toGlm(a), toGlm(b)));
        requireVecNear(xe::max(a, b), glm::max(toGlm(a), toGlm(b)));
    }
}

TEST_CASE("vec4 arithmetic and dot match glm on random inputs", "[vector][xcheck][vec4]") {
    auto rng = seededRng();

    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec4 a = randomVec4(rng, -1000.0f, 1000.0f);
        const xe::vec4 b = randomVec4(rng, -1000.0f, 1000.0f);
        const glm::vec4 ga = toGlm(a);
        const glm::vec4 gb = toGlm(b);

        requireVecNear(a + b, ga + gb);
        requireVecNear(a - b, ga - gb);
        requireNear(xe::dot(a, b), glm::dot(ga, gb));
    }
}
