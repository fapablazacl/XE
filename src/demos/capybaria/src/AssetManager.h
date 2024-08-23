
#pragma once

#include <memory>
#include <string>
#include <map>
#include <optional>

class ILogger;

namespace XE {
    struct AssetPack {
        std::string mPath; 
        std::map<std::string, std::string> mAssetIdFileMap;
    };

    class Stream;
    class AssetManager {
    public:
        explicit AssetManager(ILogger *logger);

        virtual ~AssetManager() = default;

        void addAssetPack(const std::string &name, const AssetPack& assetPack);

        std::unique_ptr<Stream> open(const std::string& assetId);

        std::optional<std::string> loadString(const std::string& assetId);

    private:
        std::map<std::string, AssetPack> mAssetPackMap;
        ILogger* mLogger = nullptr;
    };
}
