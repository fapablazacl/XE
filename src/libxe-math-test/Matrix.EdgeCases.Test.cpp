/**
 * @file Matrix.EdgeCases.Test.cpp
 * @brief Pin down xe::mat4 behaviour at numerical edge cases.
 *
 * Explicit scenarios that random sampling rarely hits cleanly: degenerate
 * view matrices, near-singular inversion inputs, reflection determinants.
 * Each case documents current behaviour so a regression is loud.
 */

#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "xe/math/Common.h"
#include "xe/math/Matrix.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

TEST_CASE("determinant of a pure reflection is -1", "[matrix][edge][determinant]") {
    // Negating one axis flips handedness; the signed volume is therefore -1.
    const xe::mat4 flipX = xe::scale(xe::vec3{-1.0f, 1.0f, 1.0f});
    REQUIRE_THAT(xe::determinant(flipX), WithinRel(-1.0f, 1e-6f));

    const xe::mat4 flipY = xe::scale(xe::vec3{1.0f, -1.0f, 1.0f});
    REQUIRE_THAT(xe::determinant(flipY), WithinRel(-1.0f, 1e-6f));

    const xe::mat4 flipZ = xe::scale(xe::vec3{1.0f, 1.0f, -1.0f});
    REQUIRE_THAT(xe::determinant(flipZ), WithinRel(-1.0f, 1e-6f));
}

TEST_CASE("determinant of an all-axis flip is -1", "[matrix][edge][determinant]") {
    // Three reflections compose to -1 * -1 * -1 = -1.
    const xe::mat4 flipAll = xe::scale(xe::vec3{-1.0f, -1.0f, -1.0f});
    REQUIRE_THAT(xe::determinant(flipAll), WithinRel(-1.0f, 1e-6f));
}

TEST_CASE("inverse of a near-singular matrix produces very large components", "[matrix][edge][inverse]") {
    // Scale one axis by 1e-8: determinant collapses to ~1e-8 and the
    // inverse along that axis blows up to ~1e8. We don't want NaN/Inf in
    // this regime, just a finite (if huge) matrix — so the caller can at
    // least detect it with a magnitude check.
    const xe::mat4 m = xe::scale(xe::vec3{1e-8f, 1.0f, 1.0f});
    const xe::mat4 inv = xe::inverse(m);

    REQUIRE(std::isfinite(inv(0, 0)));
    REQUIRE(inv(0, 0) > 1e7f);
    REQUIRE_THAT(inv(1, 1), WithinRel(1.0f, 1e-6f));
    REQUIRE_THAT(inv(2, 2), WithinRel(1.0f, 1e-6f));
}

TEST_CASE("inverse of a zero-scale matrix produces non-finite values", "[matrix][edge][inverse]") {
    // When the determinant is exactly zero the matrix has no inverse.
    // Document the current behaviour (division by zero yields NaN/Inf) so
    // a future refactor that adds a singular-matrix assertion has to touch
    // this test deliberately.
    const xe::mat4 m = xe::scale(xe::vec3{0.0f, 1.0f, 1.0f});
    const xe::mat4 inv = xe::inverse(m);
    // Exactly one of the diagonal slots should be non-finite (the inverted
    // zero-scale axis); the rest should remain finite.
    const bool anyNonFinite = !std::isfinite(inv(0, 0)) || !std::isfinite(inv(1, 1)) || !std::isfinite(inv(2, 2));
    REQUIRE(anyNonFinite);
}

TEST_CASE("lookAtRH with eye==center produces a matrix containing NaN", "[matrix][edge][lookAt]") {
    // Forward vector = center - eye = zero, which cannot be normalized.
    // The NaN then propagates through the basis computation. Pinned so a
    // future safety-net (e.g. asserting degenerate input) would have to
    // update this test on purpose.
    const xe::vec3 same{1.0f, 2.0f, 3.0f};
    const xe::vec3 up{0.0f, 1.0f, 0.0f};
    const xe::mat4 v = xe::lookAtRH(same, same, up);

    bool anyNaN = false;
    for (int i = 0; i < 16; ++i) {
        if (std::isnan(v.data()[i])) {
            anyNaN = true;
            break;
        }
    }
    REQUIRE(anyNaN);
}

TEST_CASE("transpose of an identity-derived matrix round-trips", "[matrix][edge][transpose]") {
    const xe::mat4 m = xe::translate(xe::vec3{1.0f, 2.0f, 3.0f}) * xe::scale(xe::vec3{2.0f, 3.0f, 4.0f});
    const xe::mat4 tt = xe::transpose(xe::transpose(m));
    for (int i = 0; i < 16; ++i) {
        REQUIRE(tt.data()[i] == m.data()[i]);
    }
}
