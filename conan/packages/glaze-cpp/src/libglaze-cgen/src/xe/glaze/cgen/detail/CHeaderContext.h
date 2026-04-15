#pragma once

#include "xe/glaze/codegen/ExtensionFilter.h"
#include "xe/glaze/codegen/TypeMapper.h"
#include "xe/glaze/docparser/DocIndex.h"
#include "xe/glaze/model/Registry.h"

#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <vector>

namespace xe::glaze::cgen::detail {

/**
 * @brief Build the nlohmann::json context consumed by the gl.h.inja template.
 * Mirrors glaze/generators/c_generator.py::CGenerator._header_context.
 */
nlohmann::json buildHeaderContext(
    const model::Registry &registry,
    const std::vector<std::reference_wrapper<const model::Feature>> &features,
    const std::vector<codegen::ExtensionEmission> &extensionEmissions,
    const std::string &api,
    const std::string &version,
    const codegen::TypeMapper &typeMapper,
    const docparser::DocIndex &docs = {});

} // namespace xe::glaze::cgen::detail
