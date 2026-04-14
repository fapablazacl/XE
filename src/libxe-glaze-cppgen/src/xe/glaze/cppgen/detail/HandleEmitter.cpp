#include "xe/glaze/cppgen/detail/HandleEmitter.h"

#include "xe/glaze/cppgen/detail/ParamFormat.h"
#include "xe/glaze/cppgen/detail/PatternDetector.h"
#include "xe/glaze/model/StringUtils.h"
#include "xe/glaze/model/TypeDecl.h"

#include <cctype>
#include <string>

namespace xe::glaze::cppgen::detail {

namespace {

//! Turn a raw gl command name into a lowerCamel method name. Mirrors
//! cpp_generator.py::_handle_method_name — we simply strip "gl" and
//! lowercase the first remaining letter. The class portion is NOT stripped
//! here because legacy handle methods keep names like bindBuffer(), linkProgram().
std::string handleMethodName(const std::string &glName) {
    if (glName.size() < 3 || glName[0] != 'g' || glName[1] != 'l') {
        return glName;
    }
    std::string out;
    out.reserve(glName.size() - 2);
    out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(glName[2]))));
    out.append(glName.substr(3));
    return out;
}

//! True when a command is a legacy handle method for the given class: first
//! param has class_ == classStr AND the command name does NOT start with glNamed*.
bool isHandleMethodForClass(const model::Command &command, const std::string &classStr) {
    if (command.params.empty()) {
        return false;
    }
    const auto &first = command.params.front();
    if (!isUintHandle(first) || !model::hasClass(first) || *first.classStr != classStr) {
        return false;
    }
    return command.name.rfind("glNamed", 0) != 0;
}

struct HandleMethodStrings {
    std::string paramsStr;
    std::string callArgsStr;
};

HandleMethodStrings buildHandleStrings(const model::Command &command, const EmitterContext &ctx) {
    HandleMethodStrings s;
    // Legacy handle methods delegate to the free functor, so the first arg
    // is always *this (passed as the wrapped handle id).
    s.callArgsStr = "m_id";
    bool skippedFirst = false;
    for (const auto &param : command.params) {
        if (!skippedFirst) {
            skippedFirst = true;
            continue;
        }
        if (!s.paramsStr.empty()) {
            s.paramsStr.append(", ");
        }
        s.paramsStr.append(
            generateParamDecl(param, command, ctx.handleClasses, ctx.groupRename, ctx.bitmaskGroups));
        s.callArgsStr.append(", ");
        s.callArgsStr.append(param.name);
    }
    return s;
}

} // namespace

nlohmann::json buildHandleClasses(const model::Registry &registry,
                                  const model::ConsolidatedRequire &consolidated,
                                  const EmitterContext &ctx) {
    nlohmann::json classes = nlohmann::json::array();

    for (const auto &[classStr, handleType] : ctx.handleClasses) {
        nlohmann::json methods = nlohmann::json::array();
        for (const auto &command : registry.commands()) {
            if (consolidated.commands.count(command.name) == 0) {
                continue;
            }
            if (!isHandleMethodForClass(command, classStr)) {
                continue;
            }

            const auto strings = buildHandleStrings(command, ctx);

            // Return type mirrors what the free functor returns — reuse the
            // return-type logic from the functor emitter by inlining its rules.
            std::string returnType;
            if (isStringReturnCommand(command)) {
                returnType = "std::string";
            } else if (isUniformLocationReturn(command)) {
                returnType = "UniformLocation";
            } else if (isAttribLocationReturn(command)) {
                returnType = "AttribLocation";
            } else {
                returnType = model::toCString(command.returnType);
            }

            int versionInt = 0;
            if (const auto it = ctx.commandVersionMap.find(command.name);
                it != ctx.commandVersionMap.end()) {
                try {
                    versionInt = model::versionToInt(it->second);
                } catch (const std::invalid_argument &) {
                    versionInt = 0;
                }
            }

            methods.push_back(nlohmann::json{
                {"name", handleMethodName(command.name)},
                {"return_type", returnType},
                {"params_str", strings.paramsStr},
                {"call_args_str", strings.callArgsStr},
                {"gl_name", command.name},
                {"functor_name", ctx.nameTransform.transformCommandName(command.name)},
                {"version_int", versionInt},
                {"is_void_return", returnType == "void"},
            });
        }

        if (methods.empty()) {
            continue;
        }

        classes.push_back(nlohmann::json{
            {"class_name", handleType},
            {"handle_type", handleType},
            {"methods", methods},
        });
    }

    return classes;
}

} // namespace xe::glaze::cppgen::detail
