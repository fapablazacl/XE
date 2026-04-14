#pragma once

#include "xe/glaze/model/Registry.h"

#include <string>
#include <string_view>

namespace xe::glaze::producer {

/**
 * @brief Parses the Khronos OpenGL XML registry into an in-memory Registry.
 * Extended description: this is the only class in the port that knows about
 * the underlying XML backend (pugixml). Callers receive a plain-value
 * xe::glaze::model::Registry and never see any DOM types, which lets the
 * backend be swapped without breaking any consumers.
 */
class RegistryLoader {
public:
    /**
     * @brief Load and parse a gl.xml file from disk.
     * @param path filesystem path to the registry file
     * @return fully populated Registry with all indices built
     * @throws xe::glaze::producer::ParseError on I/O or structural failure
     */
    xe::glaze::model::Registry loadFromFile(std::string_view path) const;

    /**
     * @brief Parse an already-loaded XML string.
     * @param xml UTF-8 encoded XML text
     * @return fully populated Registry
     * @throws xe::glaze::producer::ParseError on structural failure
     */
    xe::glaze::model::Registry loadFromString(std::string_view xml) const;
};

} // namespace xe::glaze::producer
