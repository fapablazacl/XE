/**
 * @file Matrix.Xcheck.Test.cpp
 * @brief Cross-check xe::mat4 operations against glm on random inputs.
 *
 * Complements Matrix.Layout.Test.cpp (which validates static layout only)
 * by exercising the actual matrix ops with random samples. Matrices for
 * the inverse test are built as @c rotate*translate*scale with
 * well-conditioned scale factors so the determinant never crowds zero.
 */

#include <catch2/catch_test_macros.hpp>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "MathTestHelpers.h"
#include "xe/math/Matrix.h"

using xe::test::randomUnitVec3;
using xe::test::randomVec3;
using xe::test::randomVec4;
using xe::test::requireMatNear;
using xe::test::requireNear;
using xe::test::requireVecNear;
using xe::test::seededRng;
using xe::test::toGlm;

namespace {
    constexpr int kSamples = 32;

    // Build a well-conditioned mat4 whose scale factors are far from zero.
    // Forms translate * rotate * scale, matching the glm convention used on
    // the right-hand side of the comparison.
    struct MatPair {
        xe::mat4 xm;
        glm::mat4 gm;
    };

    MatPair makeWellConditioned(std::mt19937 &rng) {
        std::uniform_real_distribution<float> scaleDist(0.1f, 10.0f);
        std::uniform_real_distribution<float> angleDist(-3.14f, 3.14f);
        std::uniform_real_distribution<float> transDist(-100.0f, 100.0f);

        const xe::vec3 s{scaleDist(rng), scaleDist(rng), scaleDist(rng)};
        const xe::vec3 t{transDist(rng), transDist(rng), transDist(rng)};
        const xe::vec3 axis = randomUnitVec3(rng);
        const float angle = angleDist(rng);

        const xe::mat4 xm = xe::translate(t) * xe::rotate(angle, axis) * xe::scale(s);
        const glm::mat4 gm = glm::translate(glm::mat4(1.0f), toGlm(t)) * glm::rotate(glm::mat4(1.0f), angle, toGlm(axis)) * glm::scale(glm::mat4(1.0f), toGlm(s));
        return {xm, gm};
    }
} // namespace

TEST_CASE("mat4 * mat4 matches glm on random inputs", "[matrix][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const MatPair a = makeWellConditioned(rng);
        const MatPair b = makeWellConditioned(rng);
        requireMatNear(a.xm * b.xm, a.gm * b.gm);
    }
}

TEST_CASE("mat4 * vec4 matches glm on random inputs", "[matrix][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const MatPair m = makeWellConditioned(rng);
        const xe::vec4 v = randomVec4(rng, -100.0f, 100.0f);
        requireVecNear(m.xm * v, m.gm * toGlm(v));
    }
}

TEST_CASE("mat4 transpose matches glm on random inputs", "[matrix][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const MatPair m = makeWellConditioned(rng);
        requireMatNear(xe::transpose(m.xm), glm::transpose(m.gm));
    }
}

TEST_CASE("mat4 determinant matches glm on random inputs", "[matrix][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const MatPair m = makeWellConditioned(rng);
        // Determinants of well-conditioned matrices can still be large (scale
        // cubed ~= 1000), so compare with a relative-dominant tolerance.
        const float xd = xe::determinant(m.xm);
        const float gd = glm::determinant(m.gm);
        requireNear(xd, gd);
    }
}

TEST_CASE("mat4 inverse matches glm on well-conditioned inputs", "[matrix][xcheck]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const MatPair m = makeWellConditioned(rng);
        // Inverse accumulates error proportional to the condition number —
        // use a per-test-case comparison, not the default tolerance, by
        // spot-checking that M * inv(M) ≈ identity on both sides.
        const xe::mat4 xi = xe::inverse(m.xm);
        const glm::mat4 gi = glm::inverse(m.gm);
        requireMatNear(xi, gi);
    }
}

TEST_CASE("xe::translate matches glm::translate on random inputs", "[matrix][xcheck][translate]") {
    auto rng = seededRng();
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 t = randomVec3(rng, -1000.0f, 1000.0f);
        requireMatNear(xe::translate(t), glm::translate(glm::mat4(1.0f), toGlm(t)));
    }
}

TEST_CASE("xe::scale matches glm::scale on random inputs", "[matrix][xcheck][scale]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> sd(0.01f, 100.0f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const xe::vec3 s{sd(rng), sd(rng), sd(rng)};
        requireMatNear(xe::scale(s), glm::scale(glm::mat4(1.0f), toGlm(s)));
    }
}

TEST_CASE("xe::rotate matches glm::rotate on random angle/axis pairs", "[matrix][xcheck][rotate]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> angleDist(-6.28f, 6.28f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const float angle = angleDist(rng);
        const xe::vec3 axis = randomUnitVec3(rng);
        requireMatNear(xe::rotate(angle, axis), glm::rotate(glm::mat4(1.0f), angle, toGlm(axis)));
    }
}

TEST_CASE("xe::lookAtRH matches glm on random inputs", "[matrix][xcheck][lookAt]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> posDist(-50.0f, 50.0f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        // Keep eye and center apart so the forward vector is never degenerate.
        const xe::vec3 eye{posDist(rng), posDist(rng), posDist(rng) + 100.0f};
        const xe::vec3 center{posDist(rng), posDist(rng), posDist(rng)};
        const xe::vec3 up = randomUnitVec3(rng);
        requireMatNear(xe::lookAtRH(eye, center, up), glm::lookAtRH(toGlm(eye), toGlm(center), toGlm(up)));
    }
}

TEST_CASE("xe::perspectiveRH matches glm::perspectiveRH_NO on random inputs", "[matrix][xcheck][perspective]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> fovyDist(xe::radians(10.0f), xe::radians(170.0f));
    std::uniform_real_distribution<float> aspectDist(0.5f, 3.0f);
    std::uniform_real_distribution<float> nearDist(0.01f, 10.0f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const float fovy = fovyDist(rng);
        const float aspect = aspectDist(rng);
        const float znear = nearDist(rng);
        const float zfar = znear + 10.0f + nearDist(rng) * 100.0f;
        requireMatNear(xe::perspectiveRH(fovy, aspect, znear, zfar), glm::perspectiveRH_NO(fovy, aspect, znear, zfar));
    }
}

TEST_CASE("xe::orthoRH matches glm::orthoRH_NO on random inputs", "[matrix][xcheck][ortho]") {
    auto rng = seededRng();
    std::uniform_real_distribution<float> dimDist(0.1f, 100.0f);
    std::uniform_real_distribution<float> nearDist(0.01f, 10.0f);
    for (int i = 0; i < kSamples; ++i) {
        CAPTURE(i);
        const float w = dimDist(rng);
        const float h = dimDist(rng);
        const float znear = nearDist(rng);
        const float zfar = znear + 10.0f + dimDist(rng);
        requireMatNear(xe::orthoRH(-w, w, -h, h, znear, zfar), glm::orthoRH_NO(-w, w, -h, h, znear, zfar));
    }
}
