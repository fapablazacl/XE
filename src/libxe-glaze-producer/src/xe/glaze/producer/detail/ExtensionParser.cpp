#include "xe/glaze/producer/detail/ExtensionParser.h"

#include "xe/glaze/producer/detail/FeatureParser.h"
#include "xe/glaze/producer/detail/XmlHelpers.h"

namespace xe::glaze::producer::detail {

namespace {

xe::glaze::model::Extension parseExtension(const pugi::xml_node &extNode) {
    xe::glaze::model::Extension ext;
    ext.name = extNode.attribute("name").as_string();
    if (const auto supportedAttr = optionalAttr(extNode, "supported"); supportedAttr) {
        ext.supported = splitDelimited(*supportedAttr, '|');
    }
    for (const auto &child : extNode.children("require")) {
        ext.requireList.push_back(parseRequire(child));
    }
    return ext;
}

} // namespace

std::vector<xe::glaze::model::Extension> parseExtensions(const pugi::xml_node &registry) {
    std::vector<xe::glaze::model::Extension> result;
    const auto extensionsNode = registry.child("extensions");
    if (!extensionsNode) {
        return result;
    }
    for (const auto &child : extensionsNode.children("extension")) {
        result.push_back(parseExtension(child));
    }
    return result;
}

} // namespace xe::glaze::producer::detail
