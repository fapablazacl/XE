/**
 * @file MathTestHelpers.h
 * @brief Shared helpers for the xe-math test suite.
 *
 * Test-only header (not part of libxe-math's public API). Centralises the
 * glm <-> xe comparison primitives and the seeded RNG used by the
 * cross-validation and edge-case test files so every file shares the same
 * tolerances and determinism contract.
 */

#pragma once

#include <cstddef>
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "xe/math/Matrix.h"
#include "xe/math/Quaternion.h"
#include "xe/math/Vector.h"

namespace xe::test {
    //! Absolute tolerance used when the expected value is near zero.
    inline constexpr float kAbsEps = 1e-5f;

    //! Relative tolerance used when the expected value has magnitude > 1.
    inline constexpr float kRelEps = 1e-5f;

    //! Fixed RNG seed so a failing sample on one machine reproduces everywhere.
    inline constexpr std::uint32_t kRngSeed = 0x9E3779B1u;

    /**
     * @brief Build a deterministic @c std::mt19937 seeded from @ref kRngSeed.
     * Call once per TEST_CASE to get an independent, reproducible stream.
     * @return A freshly seeded @c std::mt19937.
     */
    inline std::mt19937 seededRng() {
        return std::mt19937{kRngSeed};
    }

    /**
     * @brief Require two scalars agree within a mixed abs/rel tolerance.
     * Absolute tolerance guards the near-zero regime where relative
     * tolerance is meaningless; otherwise the tighter of the two wins.
     * @param actual Value under test.
     * @param expected Reference (typically from glm).
     */
    inline void requireNear(float actual, float expected) {
        REQUIRE_THAT(actual, Catch::Matchers::WithinAbs(expected, kAbsEps) || Catch::Matchers::WithinRel(expected, kRelEps));
    }

    /**
     * @brief Element-wise comparison of two 3-vectors.
     * Fires one @c REQUIRE_THAT per component so a mismatch points at the
     * offending index.
     * @param actual xe vector.
     * @param expected glm vector.
     */
    inline void requireVecNear(const xe::vec3 &actual, const glm::vec3 &expected) {
        for (int i = 0; i < 3; ++i) {
            CAPTURE(i);
            requireNear(actual[i], expected[i]);
        }
    }

    /**
     * @brief Element-wise comparison of two 4-vectors.
     * @param actual xe vector.
     * @param expected glm vector.
     */
    inline void requireVecNear(const xe::vec4 &actual, const glm::vec4 &expected) {
        for (int i = 0; i < 4; ++i) {
            CAPTURE(i);
            requireNear(actual[i], expected[i]);
        }
    }

    /**
     * @brief Element-wise comparison of two 4x4 matrices via their raw buffers.
     * xe::mat4 and glm::mat4 share bit-identical column-major layout so
     * walking @c data() side by side is well-defined.
     * @param actual xe matrix.
     * @param expected glm matrix.
     */
    inline void requireMatNear(const xe::mat4 &actual, const glm::mat4 &expected) {
        const float *a = actual.data();
        const float *e = glm::value_ptr(expected);
        for (int i = 0; i < 16; ++i) {
            CAPTURE(i);
            requireNear(a[i], e[i]);
        }
    }

    /**
     * @brief Element-wise comparison of two quaternions.
     * Both xe::quat and glm::quat store @c (x, y, z, w) in memory order.
     * @param actual xe quaternion.
     * @param expected glm quaternion.
     */
    inline void requireQuatNear(const xe::quat &actual, const glm::quat &expected) {
        requireNear(actual.x, expected.x);
        requireNear(actual.y, expected.y);
        requireNear(actual.z, expected.z);
        requireNear(actual.w, expected.w);
    }

    /**
     * @brief Convert an xe::vec3 to a glm::vec3 via its contiguous storage.
     * @param v xe vector.
     * @return Equivalent glm vector.
     */
    inline glm::vec3 toGlm(const xe::vec3 &v) {
        return glm::make_vec3(v.data());
    }

    /**
     * @brief Convert an xe::vec4 to a glm::vec4 via its contiguous storage.
     * @param v xe vector.
     * @return Equivalent glm vector.
     */
    inline glm::vec4 toGlm(const xe::vec4 &v) {
        return glm::make_vec4(v.data());
    }

    /**
     * @brief Convert an xe::mat4 to a glm::mat4 via its contiguous storage.
     * @param m xe matrix.
     * @return Equivalent glm matrix.
     */
    inline glm::mat4 toGlm(const xe::mat4 &m) {
        return glm::make_mat4(m.data());
    }

    /**
     * @brief Convert an xe::quat to a glm::quat.
     * Component-wise to keep the scalar-first @c glm::quat(w,x,y,z)
     * constructor contract explicit at the call site.
     * @param q xe quaternion.
     * @return Equivalent glm quaternion.
     */
    inline glm::quat toGlm(const xe::quat &q) {
        return {q.w, q.x, q.y, q.z};
    }

    /**
     * @brief Draw a random xe::vec3 whose components lie in @c [lo, hi].
     * @param rng RNG state (mutated).
     * @param lo Inclusive lower bound.
     * @param hi Inclusive upper bound.
     * @return A fresh random vector.
     */
    inline xe::vec3 randomVec3(std::mt19937 &rng, float lo, float hi) {
        std::uniform_real_distribution<float> d(lo, hi);
        return {d(rng), d(rng), d(rng)};
    }

    /**
     * @brief Draw a random xe::vec4 whose components lie in @c [lo, hi].
     * @param rng RNG state (mutated).
     * @param lo Inclusive lower bound.
     * @param hi Inclusive upper bound.
     * @return A fresh random vector.
     */
    inline xe::vec4 randomVec4(std::mt19937 &rng, float lo, float hi) {
        std::uniform_real_distribution<float> d(lo, hi);
        return {d(rng), d(rng), d(rng), d(rng)};
    }

    /**
     * @brief Draw a random unit-length xe::vec3, safe for use as a rotation axis.
     * Rejects the (extremely unlikely) zero draw and retries so the result
     * is always normalisable.
     * @param rng RNG state (mutated).
     * @return A unit-length random vector.
     */
    inline xe::vec3 randomUnitVec3(std::mt19937 &rng) {
        std::uniform_real_distribution<float> d(-1.0f, 1.0f);
        for (;;) {
            const xe::vec3 v{d(rng), d(rng), d(rng)};
            const float len2 = xe::length2(v);
            if (len2 > 1e-6f) {
                return xe::normalize(v);
            }
        }
    }
} // namespace xe::test
