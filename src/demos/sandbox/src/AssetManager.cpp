
#include "AssetManager.h"

#include <cassert>
#include <filesystem>

#include <xe/io/FileStream.h>
#include "Logger.h"

namespace XE {
    AssetManager::AssetManager(ILogger *logger) {
        assert(logger);

        mLogger = logger;
    }
    
    void AssetManager::addAssetPack(const std::string& name, const AssetPack& assetPack) {
        assert(mAssetPackMap.find(name) == mAssetPackMap.end() && "A previous asset pack with that name already exists");

        mAssetPackMap[name] = assetPack;
    }

    std::unique_ptr<Stream> AssetManager::open(const std::string& assetId) {
        mLogger->log("Searching Asset " + assetId + " in " + std::to_string(mAssetPackMap.size()) + " registered asset packs");

        namespace fs = std::filesystem;

        for (const auto& pair : mAssetPackMap) {
            mLogger->log("Looking into pack " + pair.first);

            const AssetPack& pack = pair.second;
            const auto it = pack.mAssetIdFileMap.find(assetId);

            if (it == pack.mAssetIdFileMap.end()) {
                mLogger->log("Asset wasn't found in pack " + pair.first);
                continue;
            }

            const fs::path base = fs::path(pack.mPath);
            const std::string assetFilePath = (base / fs::path(it->second)).string();

            mLogger->log("Asset found in pack " + pair.first);
            mLogger->log("Asset located in filesystem " + assetFilePath);

            return std::make_unique<FileStream>(assetFilePath, StreamFlags::Readable);
        }

        mLogger->log("Asset " + assetId + " wasn't found on any registered asset pack");

        return {};
    }

    std::optional<std::string> AssetManager::loadString(const std::string& assetId) {
        auto stream = open(assetId);

        if (!stream) {
            return {};
        }

        const int bufferSize = 1024; // Choose a suitable buffer size
        char buffer[bufferSize + 1]; // Add 1 for the null-terminator
        std::string result;

        int bytesRead = 0;
        int totalBytesRead = 0;

        do {
            // Read data from the stream in chunks
            bytesRead = stream->read(buffer, 1, bufferSize);
            totalBytesRead += bytesRead;

            // Append the read data to the result string
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            result += buffer;
        } while (bytesRead > 0);

        // If the stream is not null-terminated, you can remove the null characters from the end.
        result.resize(totalBytesRead);

        return result;
    }
}
