
#include "FileStreamSource.hpp"

#include <string>
#include <experimental/filesystem>
#include <XE/IO/FileStream.hpp>

namespace XE {
    namespace fs = std::experimental::filesystem;

    FileStreamSource::FileStreamSource(const std::string &directory) {
        fs::path path;

        if (directory == "") {
            path = fs::current_path();
        } else  {
            path = fs::path(directory);
        }

        if (!fs::exists(path)) {
            if (path.is_relative()) {
                const std::string msg = "FileStreamSource: The directory '" + path.string() + "' doesn't exist.";
                throw std::runtime_error(msg);
            } else {
                const std::string msg = 
                "FileStreamSource: The directory '" + directory + "' doesn't exist "
                    + "(current path: " + fs::current_path().string() + ").";

                throw std::runtime_error(msg);
            }
        }

        this->directory = path.string();
    }

    std::unique_ptr<Stream> FileStreamSource::open(const std::string &path) {
        const fs::path finalPath = fs::path(directory) / fs::path(path);

        return std::make_unique<FileStream>(finalPath.string(), StreamFlags::Readable);
    }

    bool FileStreamSource::exists(const std::string &path) const {
        const fs::path finalPath = fs::path(directory) / fs::path(path);

        return fs::exists(finalPath);
    }
}
