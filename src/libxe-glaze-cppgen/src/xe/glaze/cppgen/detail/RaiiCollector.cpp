#include "xe/glaze/cppgen/detail/RaiiCollector.h"

#include "xe/glaze/cppgen/detail/PatternDetector.h"

#include <string>

namespace xe::glaze::cppgen::detail {

namespace {

//! Convert a gl command name such as "glGenBuffers" into a lowerCamel method
//! name: "genBuffer" (singular when pluralized by GL convention).
std::string makeSingularGenName(const std::string &glName) {
    // glGenBuffers -> genBuffer, glCreateShader -> createShader
    std::string rest;
    if (glName.rfind("gl", 0) == 0 && glName.size() > 2) {
        rest = glName.substr(2);
    } else {
        rest = glName;
    }
    if (!rest.empty()) {
        rest.front() = static_cast<char>(std::tolower(static_cast<unsigned char>(rest.front())));
    }
    if (!rest.empty() && rest.back() == 's') {
        rest.pop_back();
    }
    return rest;
}

std::string makeSingularDeleteName(const std::string &glName) {
    std::string rest;
    if (glName.rfind("gl", 0) == 0 && glName.size() > 2) {
        rest = glName.substr(2);
    } else {
        rest = glName;
    }
    if (!rest.empty()) {
        rest.front() = static_cast<char>(std::tolower(static_cast<unsigned char>(rest.front())));
    }
    if (!rest.empty() && rest.back() == 's') {
        rest.pop_back();
    }
    return rest;
}

} // namespace

nlohmann::json collectRaiiResources(const model::Registry &registry,
                                    const model::ConsolidatedRequire &consolidated,
                                    const std::map<std::string, std::string> &handleClasses) {
    nlohmann::json out = nlohmann::json::array();
    (void)registry;

    // Index commands by GL name for quick lookup.
    for (const auto &[classStr, handleType] : handleClasses) {
        // Search for a creator: glGen<Class>s(count, out) or glCreate<Class>()
        // and a deleter: glDelete<Class>s(count, in) or glDelete<Class>(id).
        const model::Command *multiGen = nullptr;
        const model::Command *multiDel = nullptr;
        const model::Command *singleGen = nullptr;
        const model::Command *singleDel = nullptr;

        for (const auto &cmd : registry.commands()) {
            if (consolidated.commands.count(cmd.name) == 0) {
                continue;
            }
            const model::CommandParam *countParam = nullptr;
            const model::CommandParam *handleParam = nullptr;

            if (findObjectCreationParams(cmd, countParam, handleParam) &&
                handleParam->classStr == classStr) {
                multiGen = &cmd;
                continue;
            }
            if (findObjectDeletionParams(cmd, countParam, handleParam) &&
                handleParam->classStr == classStr) {
                multiDel = &cmd;
                continue;
            }
            // Singular creator: no params, returns GLuint
            if (cmd.name.rfind("glCreate", 0) == 0 && cmd.returnType.name == "GLuint" &&
                !cmd.returnType.isPointer) {
                // Match class from command name suffix, e.g. glCreateProgram -> program
                std::string suffix = cmd.name.substr(8); // drop "glCreate"
                for (auto &c : suffix) {
                    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                }
                if (suffix == classStr) {
                    // accept when params are empty OR first param has no class
                    if (cmd.params.empty() ||
                        (cmd.params.front().typeParts.size() >= 1 &&
                         !model::hasClass(cmd.params.front()))) {
                        singleGen = &cmd;
                    }
                }
            }
            // Singular deleter: single class-tagged GLuint param
            if (cmd.name.rfind("glDelete", 0) == 0 && cmd.params.size() == 1) {
                const auto &p = cmd.params.front();
                if (isUintHandle(p) && !model::isPointer(p) && model::hasClass(p) &&
                    *p.classStr == classStr) {
                    singleDel = &cmd;
                }
            }
        }

        const model::Command *chosenGen = multiGen != nullptr ? multiGen : singleGen;
        const model::Command *chosenDel = multiDel != nullptr ? multiDel : singleDel;
        if (chosenGen == nullptr || chosenDel == nullptr) {
            continue;
        }

        nlohmann::json entry;
        entry["alias"] = handleType;
        entry["handle_type"] = handleType;
        entry["gen_gl_name"] = chosenGen->name;
        entry["gen_func_name"] = makeSingularGenName(chosenGen->name);
        entry["delete_gl_name"] = chosenDel->name;
        entry["delete_func_name"] = makeSingularDeleteName(chosenDel->name);
        entry["create_params_str"] = "";
        entry["create_call_args_str"] = "";
        out.push_back(entry);
    }

    return out;
}

} // namespace xe::glaze::cppgen::detail
