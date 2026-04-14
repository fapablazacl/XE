#include "xe/glaze/cppgen/CppGenerator.h"

#include "xe/glaze/codegen/ExtensionFilter.h"
#include "xe/glaze/cppgen/detail/DsaEmitter.h"
#include "xe/glaze/cppgen/detail/EnumClassEmitter.h"
#include "xe/glaze/cppgen/detail/FunctorEmitter.h"
#include "xe/glaze/cppgen/detail/GroupRename.h"
#include "xe/glaze/cppgen/detail/HandleEmitter.h"
#include "xe/glaze/cppgen/detail/ParamFormat.h"
#include "xe/glaze/cppgen/detail/PatternDetector.h"
#include "xe/glaze/cppgen/detail/RaiiCollector.h"
#include "xe/glaze/model/StringUtils.h"

#include <inja/inja.hpp>

#include <algorithm>
#include <cctype>

namespace xe::glaze::cppgen::detail::templates {

// Defined by build-time embedded sources generated via cmake/EmbedTextFile.cmake.
extern const char *gl_hpp_inja;
extern const char *gl_handle_hpp_inja;
extern const char *raii_hpp;

} // namespace xe::glaze::cppgen::detail::templates

namespace xe::glaze::cppgen {

namespace {

inja::Environment makeEnvironment() {
    inja::Environment env;
    env.set_trim_blocks(true);
    env.set_lstrip_blocks(true);
    return env;
}

//! Build cmd_version_map — earliest version for each command in the core snapshot.
std::map<std::string, std::string>
buildCommandVersionMap(const model::Registry &registry,
                       const std::string &api,
                       const std::string &version) {
    std::map<std::string, std::string> map;
    const auto features = registry.collectFeatures(api, version);
    for (const auto &featureRef : features) {
        const auto &feature = featureRef.get();
        for (const auto &require : feature.requireList) {
            for (const auto &cmdRef : require.commands) {
                map.try_emplace(cmdRef.name, feature.number);
            }
        }
    }
    return map;
}

} // namespace

CppGenerator::CppGenerator(const model::Registry &registry,
                           std::shared_ptr<const codegen::NameTransform> nameTransform,
                           std::shared_ptr<const codegen::TypeMapper> typeMapper)
    : registry_(registry),
      nameTransform_(nameTransform ? std::move(nameTransform)
                                   : std::make_shared<codegen::DefaultNameTransform>()),
      typeMapper_(typeMapper ? std::move(typeMapper)
                             : std::make_shared<codegen::DefaultCTypeMapper>()) {}

std::map<std::string, std::string>
CppGenerator::generate(const std::string &api,
                       const std::string &version,
                       const std::set<std::string> &extensionVendors,
                       const std::set<std::string> &extensionNames) const {
    // Simplified port: extension emission is not yet wired through cppgen.
    // checkApiVersion still runs and the filters are validated via the
    // companion validateExtensionFilters() method when the facade calls it.
    (void)extensionVendors;
    (void)extensionNames;
    codegen::checkApiVersion(registry_, api, version);

    const auto consolidated = registry_.consolidate(api, version);

    // Discovery pass: walk every core command and gather handle classes and
    // enum group names. Extensions are skipped in this simplified port — they
    // can still emit functors via the canonical context builder if present
    // but no extra DSA/handle classes are discovered from them.
    std::map<std::string, std::string> handleClasses;
    std::set<std::string> groupSet;
    for (const auto &cmdName : consolidated.commands) {
        const auto *cmd = registry_.findCommand(cmdName);
        if (cmd == nullptr) {
            continue;
        }
        for (const auto &param : cmd->params) {
            if (model::hasClass(param) && detail::isUintHandle(param)) {
                const auto &classStr = *param.classStr;
                if (handleClasses.count(classStr) == 0) {
                    handleClasses[classStr] = nameTransform_->transformHandleTypeName(classStr);
                }
            }
            if (model::hasGroup(param)) {
                if (registry_.enumsForGroup(*param.group) != nullptr) {
                    groupSet.insert(*param.group);
                }
            }
        }
    }

    // Vendor-suffix rename pass.
    const auto groupRename = detail::buildGroupRename(groupSet);

    // Collision resolution: if a handle type name clashes with a clean enum
    // class name, append "Id" to the handle type to disambiguate.
    std::set<std::string> cleanGroupNames;
    for (const auto &g : groupSet) {
        cleanGroupNames.insert(detail::renamedGroup(groupRename, g));
    }
    for (auto &[classStr, handleName] : handleClasses) {
        if (cleanGroupNames.count(handleName) != 0) {
            handleName += "Id";
        }
    }

    // Handle types block: one distinct CamelCase name per handle class, sorted.
    std::set<std::string> handleNameSet;
    for (const auto &[_, name] : handleClasses) {
        handleNameSet.insert(name);
    }
    nlohmann::json handleTypesArr = nlohmann::json::array();
    for (const auto &name : handleNameSet) {
        handleTypesArr.push_back(nlohmann::json{{"name", name}});
    }

    // Location-type usage scan.
    bool needUniformLocation = false;
    bool needAttribLocation = false;
    for (const auto &cmdName : consolidated.commands) {
        const auto *cmd = registry_.findCommand(cmdName);
        if (cmd == nullptr) {
            continue;
        }
        if (detail::isUniformLocationReturn(*cmd)) {
            needUniformLocation = true;
        }
        if (detail::isAttribLocationReturn(*cmd)) {
            needAttribLocation = true;
        }
        for (const auto &param : cmd->params) {
            if (detail::isUniformLocationParam(*cmd, param)) {
                needUniformLocation = true;
            }
            if (detail::isAttribLocationParam(*cmd, param)) {
                needAttribLocation = true;
            }
        }
    }
    nlohmann::json locationTypesArr = nlohmann::json::array();
    if (needAttribLocation) {
        locationTypesArr.push_back(nlohmann::json{{"name", "AttribLocation"}});
    }
    if (needUniformLocation) {
        locationTypesArr.push_back(nlohmann::json{{"name", "UniformLocation"}});
    }

    // Bitmask set (raw group name -> true).
    std::set<std::string> bitmaskGroups;
    for (const auto &g : groupSet) {
        if (registry_.isBitmaskGroup(g)) {
            bitmaskGroups.insert(g);
        }
    }

    // Shared emitter context.
    detail::EmitterContext emitterCtx{
        *nameTransform_,
        handleClasses,
        groupRename,
        bitmaskGroups,
        buildCommandVersionMap(registry_, api, version),
    };

    // Enum classes block.
    nlohmann::json enumClassesArr = nlohmann::json::array();
    for (const auto &rawGroup : groupSet) {
        const auto *enumPtrs = registry_.enumsForGroup(rawGroup);
        if (enumPtrs == nullptr) {
            continue;
        }
        std::vector<const model::Enum *> filtered;
        filtered.reserve(enumPtrs->size());
        for (const auto *e : *enumPtrs) {
            if (consolidated.enums.count(e->name) != 0) {
                filtered.push_back(e);
            }
        }
        if (filtered.empty()) {
            continue;
        }
        const auto cleanName = detail::renamedGroup(groupRename, rawGroup);
        const bool isBitmask = bitmaskGroups.count(rawGroup) != 0;
        enumClassesArr.push_back(detail::buildEnumClassContext(
            rawGroup, cleanName, isBitmask, filtered, *nameTransform_));
    }

    // Functor block.
    std::vector<const model::Command *> sortedCommands;
    for (const auto &cmdName : consolidated.commands) {
        if (const auto *cmd = registry_.findCommand(cmdName); cmd != nullptr) {
            sortedCommands.push_back(cmd);
        }
    }
    std::sort(sortedCommands.begin(), sortedCommands.end(),
              [](const model::Command *a, const model::Command *b) { return a->name < b->name; });
    const auto functorsArr = detail::buildFunctors(sortedCommands, emitterCtx);

    // DSA classes.
    const auto dsaClassesArr = detail::buildDsaClasses(registry_, consolidated, emitterCtx);

    // Handle classes (legacy, non-Named*).
    const auto handleClassesArr = detail::buildHandleClasses(registry_, consolidated, emitterCtx);

    // RAII resources.
    const auto raiiResourcesArr = detail::collectRaiiResources(registry_, consolidated, handleClasses);

    // Handle wrapper RAII list: pair each raii resource with its handle-class
    // wrapper type name so gl_handle.hpp.inja can emit Traits specializations.
    nlohmann::json handleRaiiArr = nlohmann::json::array();
    for (const auto &r : raiiResourcesArr) {
        for (const auto &hc : handleClassesArr) {
            if (hc["handle_type"] == r["handle_type"]) {
                nlohmann::json entry = r;
                entry["class_name"] = hc["class_name"];
                handleRaiiArr.push_back(entry);
                break;
            }
        }
    }

    // Assemble the template contexts.
    std::string apiUpper = api;
    for (auto &c : apiUpper) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    const std::string generationHeader =
        std::string{"Auto-generated by Glaze — DO NOT EDIT.\nAPI: "} + api + " " + version +
        " | Language: C++";
    std::string generationHeaderFormatted;
    generationHeaderFormatted.reserve(generationHeader.size() + 16);
    for (char ch : generationHeader) {
        if (ch == '\n') {
            generationHeaderFormatted.append("\n * ");
        } else {
            generationHeaderFormatted.push_back(ch);
        }
    }

    nlohmann::json hppCtx = {
        {"api", api},
        {"api_upper", apiUpper},
        {"generation_header", generationHeader},
        {"generation_header_formatted", generationHeaderFormatted},
        {"handle_types", handleTypesArr},
        {"location_types", locationTypesArr},
        {"enum_classes", enumClassesArr},
        {"functors", functorsArr},
        {"dsa_classes", dsaClassesArr},
        {"raii_resources", raiiResourcesArr},
    };

    nlohmann::json handleCtx = {
        {"api", api},
        {"api_upper", apiUpper},
        {"generation_header", generationHeader},
        {"generation_header_formatted", generationHeaderFormatted},
        {"handle_classes", handleClassesArr},
        {"handle_raii_resources", handleRaiiArr},
    };

    auto env = makeEnvironment();
    std::map<std::string, std::string> result;
    result["include/glaze/" + api + ".hpp"] =
        env.render(detail::templates::gl_hpp_inja, hppCtx);
    result["include/glaze/" + api + "_handle.hpp"] =
        env.render(detail::templates::gl_handle_hpp_inja, handleCtx);
    result["include/glaze/raii.hpp"] = detail::templates::raii_hpp;
    return result;
}

std::vector<std::string>
CppGenerator::validateExtensionFilters(const std::string &api,
                                       const std::set<std::string> &extensionVendors,
                                       const std::set<std::string> &extensionNames) const {
    return codegen::validateExtensionFilters(registry_, api, extensionVendors, extensionNames);
}

} // namespace xe::glaze::cppgen
