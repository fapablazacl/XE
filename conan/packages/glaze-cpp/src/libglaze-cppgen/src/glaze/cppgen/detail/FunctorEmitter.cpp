#include "glaze/cppgen/detail/FunctorEmitter.h"

#include "glaze/cppgen/detail/OverloadEmitter.h"
#include "glaze/cppgen/detail/ParamFormat.h"
#include "glaze/cppgen/detail/PatternDetector.h"
#include "glaze/model/StringUtils.h"
#include "glaze/model/TypeDecl.h"

#include <cctype>
#include <sstream>

namespace glaze::cppgen::detail {

namespace {

std::string functorStructName(const std::string &funcName) {
    std::string out = funcName;
    if (!out.empty()) {
        out.front() = static_cast<char>(std::toupper(static_cast<unsigned char>(out.front())));
    }
    return out + "Fn";
}

//! Build the parameter-list / call-args strings for the canonical overload.
struct OverloadStrings {
    std::string paramsStr;
    std::string callArgsStr;
};

OverloadStrings buildCanonicalStrings(const model::Command &command,
                                      const EmitterContext &ctx) {
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

//! Build the pre-composed body string for the canonical operator() overload.
//! Mirrors the three branches the template used to hard-code (void / handle /
//! value return) so we can hand the emitted body straight to the template.
std::string buildCanonicalBody(const std::string &returnType,
                               const std::string &rawPtr,
                               const std::string &callArgsStr,
                               const model::Command &command) {
    const bool isVoid = returnType == "void";
    const bool isHandle = !isVoid && returnType != "std::string" &&
                          !command.returnType.isPointer &&
                          command.returnType.name == "GLuint";

    std::ostringstream body;
    if (isVoid) {
        body << "        " << rawPtr << "(" << callArgsStr << ");";
    } else if (isHandle) {
        body << "        return " << returnType << "{" << rawPtr << "("
             << callArgsStr << ")};";
    } else {
        body << "        return " << returnType << "(" << rawPtr << "("
             << callArgsStr << "));";
    }
    return body.str();
}

} // namespace

nlohmann::json buildFunctors(const std::vector<const model::Command *> &commands,
                             const EmitterContext &ctx) {
    nlohmann::json functors = nlohmann::json::array();
    for (const auto *cmd : commands) {
        const auto funcName = ctx.nameTransform.transformCommandName(cmd->name);
        const auto structName = functorStructName(funcName);

        auto overloadStrings = buildCanonicalStrings(*cmd, ctx);
        const auto returnType = functorReturnType(*cmd, ctx);
        const auto rawPtr = std::string{"glaze_"} + cmd->name;
        const auto canonicalBody = buildCanonicalBody(
            returnType, rawPtr, overloadStrings.callArgsStr, *cmd);

        int versionInt = 0;
        if (const auto it = ctx.commandVersionMap.find(cmd->name);
            it != ctx.commandVersionMap.end()) {
            try {
                versionInt = model::versionToInt(it->second);
            } catch (const std::invalid_argument &) {
                versionInt = 0;
            }
        }

        nlohmann::json canonical = {
            {"return_type", returnType},
            {"params_str", overloadStrings.paramsStr},
            {"body", canonicalBody},
            {"is_template_overload", false},
        };

        nlohmann::json overloads = nlohmann::json::array();
        overloads.push_back(std::move(canonical));
        for (auto &extra : buildExtraOverloads(*cmd, ctx)) {
            overloads.push_back(std::move(extra));
        }

        nlohmann::json fn{
            {"struct_name", structName},
            {"func_name", funcName},
            {"gl_name", cmd->name},
            {"raw_ptr", rawPtr},
            {"version_int", versionInt},
            {"overloads", std::move(overloads)},
        };

        // Doc comment fields — empty strings when no refpages dir was given.
        std::string docBrief;
        nlohmann::json docParams = nlohmann::json::array();
        if (const auto it = ctx.docs.find(cmd->name); it != ctx.docs.end()) {
            docBrief = it->second.brief;
            for (const auto &[paramName, desc] : it->second.params) {
                docParams.push_back(nlohmann::json{
                    {"name", paramName},
                    {"desc", desc},
                });
            }
        }
        fn["has_doc_brief"] = !docBrief.empty();
        fn["doc_brief"] = std::move(docBrief);
        fn["doc_params"] = std::move(docParams);

        functors.push_back(std::move(fn));
    }
    return functors;
}

} // namespace glaze::cppgen::detail
