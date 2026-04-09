/**
 * @file Matrix.Layout.Test.cpp
 * @brief Cross-check that xe::mat4's memory layout is bit-identical to glm::mat4.
 *
 * This is the guard rail for the row-major -> column-major storage flip that
 * happened during the glm-style refactor. If anything drifts here, direct
 * memcpy into a GL uniform buffer would upload the wrong matrix.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cstring>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "xe/math/Matrix.h"

using Catch::Matchers::WithinAbs;

namespace {
    // Compare every float in two 16-element matrices with tolerance.
    void requireMatricesEqual(const float *lhs, const float *rhs, float tolerance) {
        for (int i = 0; i < 16; ++i) {
            REQUIRE_THAT(lhs[i], WithinAbs(rhs[i], tolerance));
        }
    }
} // namespace

TEST_CASE("xe::mat4 has the same size as glm::mat4", "[matrix][layout]") {
    STATIC_REQUIRE(sizeof(xe::mat4) == sizeof(glm::mat4));
    STATIC_REQUIRE(sizeof(xe::mat4) == 16 * sizeof(float));
}

TEST_CASE("identity matrix memory layout matches glm", "[matrix][layout]") {
    const xe::mat4 xm(1.0f);
    const glm::mat4 gm(1.0f);
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 0.0f);
}

TEST_CASE("translation matrix memory layout matches glm", "[matrix][layout]") {
    const xe::mat4 xm = xe::translate(xe::vec3{1.0f, 2.0f, 3.0f});
    const glm::mat4 gm = glm::translate(glm::mat4(1.0f), glm::vec3{1.0f, 2.0f, 3.0f});
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-6f);
}

TEST_CASE("scale matrix memory layout matches glm", "[matrix][layout]") {
    const xe::mat4 xm = xe::scale(xe::vec3{2.0f, 3.0f, 4.0f});
    const glm::mat4 gm = glm::scale(glm::mat4(1.0f), glm::vec3{2.0f, 3.0f, 4.0f});
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-6f);
}

TEST_CASE("rotation matrix memory layout matches glm", "[matrix][layout]") {
    const float angle = xe::radians(37.5f);
    const xe::vec3 axis{0.2f, 0.8f, 0.55f};
    const xe::mat4 xm = xe::rotate(angle, axis);
    const glm::mat4 gm = glm::rotate(glm::mat4(1.0f), angle, glm::vec3{axis.x, axis.y, axis.z});
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-5f);
}

TEST_CASE("lookAtRH memory layout matches glm", "[matrix][layout]") {
    const xe::vec3 eye{5.0f, 3.0f, 10.0f};
    const xe::vec3 center{0.0f, 0.0f, 0.0f};
    const xe::vec3 up{0.0f, 1.0f, 0.0f};

    const xe::mat4 xm = xe::lookAtRH(eye, center, up);
    const glm::mat4 gm = glm::lookAtRH(glm::vec3{eye.x, eye.y, eye.z}, glm::vec3{center.x, center.y, center.z}, glm::vec3{up.x, up.y, up.z});
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-5f);
}

TEST_CASE("perspectiveRH memory layout matches glm", "[matrix][layout]") {
    const float fovy = xe::radians(60.0f);
    const float aspect = 16.0f / 9.0f;
    const float znear = 0.1f;
    const float zfar = 1000.0f;

    const xe::mat4 xm = xe::perspectiveRH(fovy, aspect, znear, zfar);
    const glm::mat4 gm = glm::perspectiveRH_NO(fovy, aspect, znear, zfar);
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-5f);
}

TEST_CASE("orthoRH memory layout matches glm", "[matrix][layout]") {
    const xe::mat4 xm = xe::orthoRH(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    const glm::mat4 gm = glm::orthoRH_NO(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    requireMatricesEqual(xm.data(), glm::value_ptr(gm), 1e-6f);
}
