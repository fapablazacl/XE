#pragma once

#include "xe/glaze/model/Feature.h"

#include <pugixml.hpp>

#include <vector>

namespace xe::glaze::producer::detail {

/**
 * @brief Parse a <require> block into a Require value.
 * Exposed so the extension parser can reuse it.
 */
xe::glaze::model::Require parseRequire(const pugi::xml_node &requireNode);

/**
 * @brief Parse every <feature> element directly under <registry>.
 * Mirrors glaze/parser.py::RegistryParser._parse_features.
 */
std::vector<xe::glaze::model::Feature> parseFeatures(const pugi::xml_node &registry);

} // namespace xe::glaze::producer::detail
