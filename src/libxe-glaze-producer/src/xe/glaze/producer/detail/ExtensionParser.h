#pragma once

#include "xe/glaze/model/Extension.h"

#include <pugixml.hpp>

#include <vector>

namespace xe::glaze::producer::detail {

/**
 * @brief Parse every <extension> child of the <extensions> block.
 * Mirrors glaze/parser.py::RegistryParser._parse_extensions.
 * @param registry the root <registry> node
 * @return parsed Extension list, empty if no <extensions> block exists
 */
std::vector<xe::glaze::model::Extension> parseExtensions(const pugi::xml_node &registry);

} // namespace xe::glaze::producer::detail
