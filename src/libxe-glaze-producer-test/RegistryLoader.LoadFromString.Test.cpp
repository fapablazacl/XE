#include "MiniRegistry.h"

#include "xe/glaze/producer/ParseError.h"
#include "xe/glaze/producer/RegistryLoader.h"

#include <catch2/catch_test_macros.hpp>

using xe::glaze::producer::ParseError;
using xe::glaze::producer::RegistryLoader;
using xe::glaze::testfixtures::kMiniXml;
using xe::glaze::testfixtures::loadMiniRegistry;

TEST_CASE("loadFromString parses MINI_XML into a populated Registry",
          "[glaze][producer][loader]") {
    const auto registry = loadMiniRegistry();
    REQUIRE(registry.types().size() == 7);
    REQUIRE(registry.enumGroups().size() == 4);
    REQUIRE(registry.commands().size() >= 17);
    REQUIRE(registry.features().size() == 6);
    REQUIRE(registry.extensions().size() == 4);
}

TEST_CASE("loadFromString throws ParseError for malformed XML",
          "[glaze][producer][loader][errors]") {
    REQUIRE_THROWS_AS(RegistryLoader{}.loadFromString("<registry><unterminated"), ParseError);
}

TEST_CASE("loadFromString throws ParseError when the root element is wrong",
          "[glaze][producer][loader][errors]") {
    REQUIRE_THROWS_AS(RegistryLoader{}.loadFromString("<?xml version=\"1.0\"?><root/>"),
                      ParseError);
}
