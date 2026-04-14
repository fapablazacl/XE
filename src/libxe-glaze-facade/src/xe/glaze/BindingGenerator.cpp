#include "xe/glaze/BindingGenerator.h"

#include "xe/glaze/cgen/CGenerator.h"
#include "xe/glaze/cppgen/CppGenerator.h"
#include "xe/glaze/producer/RegistryLoader.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace xe::glaze {

BindingGenerator::BindingGenerator() = default;
BindingGenerator::~BindingGenerator() = default;

namespace {

//! Write a file's contents to disk, creating any missing parent directories.
void writeTextFile(const std::filesystem::path &absolutePath, const std::string &content) {
    if (const auto parent = absolutePath.parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    std::ofstream out{absolutePath, std::ios::binary | std::ios::trunc};
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
}

} // namespace

GenerateResult BindingGenerator::generate(const GenerateOptions &options) const {
    // Step 1: parse the registry once and share it across every (api, lang) pair.
    producer::RegistryLoader loader;
    auto registry = loader.loadFromFile(options.registryPath.string());

    GenerateResult result;

    // Step 2: for each requested language, instantiate the generator once and
    // run it for every requested api/version, merging the outputs.
    for (const auto language : options.languages) {
        if (language == Language::C) {
            cgen::CGenerator gen{registry, options.nameTransform, options.typeMapper};
            for (const auto &request : options.apis) {
                const auto errors = gen.validateExtensionFilters(
                    request.api, options.extensionVendors, options.extensionNames);
                if (!errors.empty()) {
                    throw std::invalid_argument(errors.front());
                }
                const auto files = gen.generate(request.api, request.version,
                                                options.extensionVendors, options.extensionNames);
                for (const auto &[relPath, content] : files) {
                    const auto abs = std::filesystem::path{options.outputDir} / relPath;
                    writeTextFile(abs, content);
                    result.writtenFiles.push_back(abs);
                }
            }
        } else if (language == Language::Cpp) {
            cppgen::CppGenerator gen{registry, options.nameTransform, options.typeMapper};
            for (const auto &request : options.apis) {
                const auto errors = gen.validateExtensionFilters(
                    request.api, options.extensionVendors, options.extensionNames);
                if (!errors.empty()) {
                    throw std::invalid_argument(errors.front());
                }
                const auto files = gen.generate(request.api, request.version,
                                                options.extensionVendors, options.extensionNames);
                for (const auto &[relPath, content] : files) {
                    const auto abs = std::filesystem::path{options.outputDir} / relPath;
                    writeTextFile(abs, content);
                    result.writtenFiles.push_back(abs);
                }
            }
        }
    }

    return result;
}

std::map<std::string, std::vector<std::string>>
BindingGenerator::listApis(const std::filesystem::path &registryPath) const {
    producer::RegistryLoader loader;
    const auto registry = loader.loadFromFile(registryPath.string());
    return registry.availableApis();
}

} // namespace xe::glaze
