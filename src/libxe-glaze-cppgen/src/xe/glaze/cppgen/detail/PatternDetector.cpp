#include "xe/glaze/cppgen/detail/PatternDetector.h"

#include <array>
#include <string_view>

namespace xe::glaze::cppgen::detail {

namespace {

constexpr std::array<std::string_view, 3> kUniformLocationReturnCommands{{
    "glGetUniformLocation",
    "glGetFragDataLocation",
    "glGetFragDataIndex",
}};

constexpr std::array<std::string_view, 1> kAttribLocationReturnCommands{{
    "glGetAttribLocation",
}};

bool containsName(const std::array<std::string_view, 3> &arr, std::string_view name) {
    for (const auto &entry : arr) {
        if (entry == name) {
            return true;
        }
    }
    return false;
}

bool containsName(const std::array<std::string_view, 1> &arr, std::string_view name) {
    return arr[0] == name;
}

bool startsWith(const std::string &s, std::string_view prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

bool contains(const std::string &s, std::string_view needle) {
    return s.find(needle) != std::string::npos;
}

} // namespace

bool isUintHandle(const model::CommandParam &param) noexcept {
    const auto base = model::baseType(param);
    return base && *base == "GLuint";
}

bool isStringReturnCommand(const model::Command &command) noexcept {
    const auto &rt = command.returnType;
    if (!rt.isConst || !rt.isPointer) {
        return false;
    }
    return rt.name == "GLubyte" || rt.name == "GLchar";
}

bool findObjectCreationParams(const model::Command &command,
                              const model::CommandParam *&countOut,
                              const model::CommandParam *&outputOut) noexcept {
    countOut = nullptr;
    outputOut = nullptr;
    if (!startsWith(command.name, "glGen") && !startsWith(command.name, "glCreate")) {
        return false;
    }
    for (const auto &param : command.params) {
        const auto base = model::baseType(param);
        if (base && *base == "GLsizei" && !model::isPointer(param)) {
            countOut = &param;
        }
        if (base && *base == "GLuint" && model::isPointer(param) && !model::isConst(param) &&
            model::hasClass(param)) {
            outputOut = &param;
        }
    }
    return countOut != nullptr && outputOut != nullptr;
}

bool findObjectDeletionParams(const model::Command &command,
                              const model::CommandParam *&countOut,
                              const model::CommandParam *&inputOut) noexcept {
    countOut = nullptr;
    inputOut = nullptr;
    if (!startsWith(command.name, "glDelete")) {
        return false;
    }
    for (const auto &param : command.params) {
        const auto base = model::baseType(param);
        if (base && *base == "GLsizei" && !model::isPointer(param)) {
            countOut = &param;
        }
        if (base && *base == "GLuint" && model::isPointer(param) && model::isConst(param) &&
            model::hasClass(param)) {
            inputOut = &param;
        }
    }
    return countOut != nullptr && inputOut != nullptr;
}

bool isUniformLocationParam(const model::Command &command,
                            const model::CommandParam &param) noexcept {
    const auto base = model::baseType(param);
    if (!base || *base != "GLint" || param.name != "location") {
        return false;
    }
    return startsWith(command.name, "glUniform") || startsWith(command.name, "glProgramUniform");
}

bool isAttribLocationParam(const model::Command &command,
                           const model::CommandParam &param) noexcept {
    const auto base = model::baseType(param);
    if (!base || *base != "GLuint" || param.name != "index") {
        return false;
    }
    return contains(command.name, "VertexAttrib") || command.name == "glBindAttribLocation";
}

bool isUniformLocationReturn(const model::Command &command) noexcept {
    return containsName(kUniformLocationReturnCommands, command.name);
}

bool isAttribLocationReturn(const model::Command &command) noexcept {
    return containsName(kAttribLocationReturnCommands, command.name);
}

} // namespace xe::glaze::cppgen::detail
