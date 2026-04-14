#pragma once

#include "xe/glaze/model/Command.h"
#include "xe/glaze/model/CommandParam.h"

namespace xe::glaze::cppgen::detail {

/**
 * @brief True if a parameter's base type is GLuint — a potential object handle.
 * Mirrors cpp_generator.py::_is_uint_handle.
 */
bool isUintHandle(const model::CommandParam &param) noexcept;

/**
 * @brief True if a command returns a C string (const GLubyte* / const GLchar*).
 * Mirrors cpp_generator.py::_is_string_return_command.
 */
bool isStringReturnCommand(const model::Command &command) noexcept;

/**
 * @brief Detect a multi-object creation pattern: (GLsizei n, GLuint *out).
 * Mirrors cpp_generator.py::_find_object_creation_params.
 * @return true and populates countOut/outputOut when the pattern matches
 */
bool findObjectCreationParams(const model::Command &command,
                              const model::CommandParam *&countOut,
                              const model::CommandParam *&outputOut) noexcept;

/**
 * @brief Detect a multi-object deletion pattern: (GLsizei n, const GLuint *in).
 * Mirrors cpp_generator.py::_find_object_deletion_params.
 */
bool findObjectDeletionParams(const model::Command &command,
                              const model::CommandParam *&countOut,
                              const model::CommandParam *&inputOut) noexcept;

/**
 * @brief True if a GLint "location" parameter is a uniform location.
 * Mirrors cpp_generator.py::_is_uniform_location_param.
 */
bool isUniformLocationParam(const model::Command &command,
                            const model::CommandParam &param) noexcept;

/**
 * @brief True if a GLuint "index" parameter is a vertex-attrib location.
 * Mirrors cpp_generator.py::_is_attrib_location_param.
 */
bool isAttribLocationParam(const model::Command &command,
                           const model::CommandParam &param) noexcept;

/**
 * @brief True if the command's name appears in the known set of commands that
 *        return a uniform location (e.g. glGetUniformLocation).
 */
bool isUniformLocationReturn(const model::Command &command) noexcept;

/**
 * @brief True if the command's name appears in the known set of commands that
 *        return an attribute location (e.g. glGetAttribLocation).
 */
bool isAttribLocationReturn(const model::Command &command) noexcept;

} // namespace xe::glaze::cppgen::detail
