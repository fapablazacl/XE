/**
 * @file Quaternion.Xcheck.Test.cpp
 * @brief Cross-check xe::quat operations against glm::quat on random inputs.
 *
 * xe::quat and glm::quat share the same (x, y, z, w) memory layout and the
 * same scalar-first (w, x, y, z) constructor, so the comparison is direct.
 */

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "MathTestHelpers.h"
#include "xe/math/Quaternion.h"

using xe::test::randomUnitVec3;
using xe::test::randomVec3;
using xe::test::requireNear;
using xe::test::requireQuatNear;
using xe::test::requireVecNear;
using xe::test::seededRng;
using xe::test::toGlm;

namespace {
    constexpr int kSamples = 64;

    xe::quat randomUnitQuat(std::mt19937 &rng) {
        std::uniform_real_distribution<float> angleDist(-6.28f, 6.28f);
        return xe::angleAxis(angleDist(rng), randomUnitVec3(rng));
    }
} // namespace

TEST_CASE("angleAxis matches glm::angleAxis on random inputs", "[quat][xcheck][angleAxis]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> angleDist(-6.28f, 6.28f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const float angle = angleDist(rng);
        const xe::vec3 axis = randomUnitVec3(rng);
        requireQuatNear(xe::angleAxis(angle, axis), glm::angleAxis(angle, toGlm(axis)));
    }
}

TEST_CASE("quat * quat (Hamilton product) matches glm on random inputs", "[quat][xcheck][mul]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::quat a = randomUnitQuat(rng);
        const xe::quat b = randomUnitQuat(rng);
        requireQuatNear(a * b, toGlm(a) * toGlm(b));
    }
}

TEST_CASE("quat * vec3 matches glm on random inputs", "[quat][xcheck][rotate]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::quat q = randomUnitQuat(rng);
        const xe::vec3 v = randomVec3(rng, -100.0f, 100.0f);
        requireVecNear(q * v, toGlm(q) * toGlm(v));
    }
}

TEST_CASE("quat conjugate / dot / length / normalize match glm on random inputs", "[quat][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::quat a = randomUnitQuat(rng);
        const xe::quat b = randomUnitQuat(rng);
        const glm::quat ga = toGlm(a);
        const glm::quat gb = toGlm(b);

        requireQuatNear(xe::conjugate(a), glm::conjugate(ga));
        requireNear(xe::dot(a, b), glm::dot(ga, gb));
        requireNear(xe::length(a), glm::length(ga));
        requireQuatNear(xe::normalize(a), glm::normalize(ga));
    }
}
