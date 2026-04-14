#include "xe/glaze/cppgen/detail/FunctorEmitter.h"

#include "xe/glaze/cppgen/detail/ParamFormat.h"
#include "xe/glaze/cppgen/detail/PatternDetector.h"
#include "xe/glaze/model/StringUtils.h"
#include "xe/glaze/model/TypeDecl.h"

#include <cctype>

namespace xe::glaze::cppgen::detail {

namespace {

std::string functorStructName(const std::string &funcName) {
    std::string out = funcName;
    if (!out.empty()) {
        out.front() = static_cast<char>(std::toupper(static_cast<unsigned char>(out.front())));
    }
    return out + "Fn";
}

//! Build the parameter-list / call-args strings for one overload.
struct OverloadStrings {
    std::string paramsStr;
    std::string callArgsStr;
};

OverloadStrings buildOverloadStrings(const model::Command &command, const EmitterContext &ctx) {
    OverloadStrings s;
    bool first = true;
    for (const auto &param : command.params) {
        if (!first) {
            s.paramsStr.append(", ");
            s.callArgsStr.append(", ");
        }
        s.paramsStr.append(
            generateParamDecl(param, command, ctx.handleClasses, ctx.groupRename, ctx.bitmaskGroups));
        s.callArgsStr.append(
            generateCallArg(param, command, ctx.handleClasses, ctx.groupRename, ctx.bitmaskGroups));
        first = false;
    }
    return s;
}

//! Determine the return type spelling for a command, substituting the handle
//! type when a singular creator returns a handle whose class we know about.
std::string functorReturnType(const model::Command &command, const EmitterContext &ctx) {
    if (isStringReturnCommand(command)) {
        return "std::string";
    }
    if (isUniformLocationReturn(command)) {
        return "UniformLocation";
    }
    if (isAttribLocationReturn(command)) {
        return "AttribLocation";
    }
    // Singular create*: glCreateProgram -> Program. We match on the class_
    // derived from the command-name suffix instead of a first-param class
    // attribute because create* commands frequently have no params.
    if (command.name.rfind("glCreate", 0) == 0 && command.returnType.name == "GLuint" &&
        !command.returnType.isPointer) {
        std::string suffix = command.name.substr(8);
        std::string lower;
        for (char ch : suffix) {
            lower.push_back(
                static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
        const auto it = ctx.handleClasses.find(lower);
        if (it != ctx.handleClasses.end()) {
            return it->second;
        }
    }
    return model::toCString(command.returnType);
}

} // namespace

nlohmann::json buildFunctors(const std::vector<const model::Command *> &commands,
                             const EmitterContext &ctx) {
    nlohmann::json functors = nlohmann::json::array();
    for (const auto *cmd : commands) {
        const auto funcName = ctx.nameTransform.transformCommandName(cmd->name);
        const auto structName = functorStructName(funcName);

        auto overloadStrings = buildOverloadStrings(*cmd, ctx);
        const auto returnType = functorReturnType(*cmd, ctx);

        int versionInt = 0;
        if (const auto it = ctx.commandVersionMap.find(cmd->name);
            it != ctx.commandVersionMap.end()) {
            try {
                versionInt = model::versionToInt(it->second);
            } catch (const std::invalid_argument &) {
                versionInt = 0;
            }
        }

        nlohmann::json overload = {
            {"return_type", returnType},
            {"params_str", overloadStrings.paramsStr},
            {"call_args_str", overloadStrings.callArgsStr},
            {"gl_name", cmd->name},
            {"raw_ptr", "glaze_" + cmd->name},
            {"is_void_return", returnType == "void"},
            {"is_handle_return", returnType != "void" && returnType != "std::string" &&
                                     !cmd->returnType.isPointer &&
                                     cmd->returnType.name == "GLuint"},
        };

        functors.push_back(nlohmann::json{
            {"struct_name", structName},
            {"func_name", funcName},
            {"gl_name", cmd->name},
            {"version_int", versionInt},
            {"overloads", nlohmann::json::array({overload})},
        });
    }
    return functors;
}

} // namespace xe::glaze::cppgen::detail
