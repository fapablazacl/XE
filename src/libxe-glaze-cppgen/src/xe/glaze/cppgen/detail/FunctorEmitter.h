#pragma once

#include "xe/glaze/codegen/NameTransform.h"
#include "xe/glaze/docparser/DocIndex.h"
#include "xe/glaze/model/Command.h"

#include <map>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <vector>

namespace xe::glaze::cppgen::detail {

/**
 * @brief Shared context passed to FunctorEmitter / DsaEmitter / HandleEmitter.
 * Extended description: groups the small amount of ambient state that every
 * context builder needs — naming and type-substitution policies plus the
 * discovery maps (handleClasses, groupRename, bitmaskGroups) the orchestrator
 * computed in its single pass over the included commands.
 */
struct EmitterContext {
    const codegen::NameTransform &nameTransform;
    //! Enclosing namespace for generated code ("gl", "gles2", ...). Used by
    //! handle methods when emitting delegations back into the free functors.
    std::string api;
    std::map<std::string, std::string> handleClasses;
    std::map<std::string, std::string> groupRename;
    std::set<std::string> bitmaskGroups;
    //! gl command name -> earliest feature version number ("1.0", "3.3", ...)
    std::map<std::string, std::string> commandVersionMap;
    //! Optional Doxygen-style brief/param documentation. Empty when the
    //! caller did not request --refpages-dir and all doc fields on the
    //! emitted JSON fall back to empty strings.
    docparser::DocIndex docs;
};

/**
 * @brief Build the list of functor JSON contexts consumed by gl.hpp.inja.
 * Extended description: walks commands, synthesizes one functor struct per
 * command family (one per gl command in this simplified port), and produces
 * a canonical operator() overload context. The operator() takes C++
 * parameter types from TypeMapper / substitutions and forwards to the raw
 * glaze_<name> function pointer defined in the C header.
 *
 * @param commands the commands to emit functors for
 * @param ctx the shared emitter context (naming, type mapping, discovery)
 */
nlohmann::json buildFunctors(const std::vector<const model::Command *> &commands,
                             const EmitterContext &ctx);

} // namespace xe::glaze::cppgen::detail
