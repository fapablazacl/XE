
#include "StreamSource.hpp"
#include "FileStream.hpp"
#include <experimental/filesystem>

namespace XE {
    namespace fs = std::experimental::filesystem;

    class FileStreamSource : public StreamSource {
    public:
        FileStreamSource(const std::string &directory) {
            this->directory = fs::path(directory);

            if (!fs::exists(this->directory)) {
                if (this->directory.is_relative()) {
                    const std::string msg = "FileStreamSource: The directory '" + directory + "' doesn't exist.";
                    throw std::runtime_error(msg);
                } else {
                    const std::string msg = 
                    "FileStreamSource: The directory '" + directory + "' doesn't exist "
                    + "(current path: " + fs::current_path().string() + ").";

                    throw std::runtime_error(msg);
                }
            }
        }

        virtual std::unique_ptr<Stream> open(const std::string &path) override {
            const fs::path finalPath = directory / fs::path(path);

            return std::make_unique<FileStream>(finalPath.string(), StreamFlags::Readable);
        }

    private:
        fs::path directory;
    };


    StreamSource::~StreamSource() {}

    std::unique_ptr<StreamSource> StreamSource::create() {
        const std::string currentPath = fs::current_path().string();

        return StreamSource::create(currentPath);
    }

    std::unique_ptr<StreamSource> StreamSource::create(const std::string &directory) {
        return std::make_unique<FileStreamSource>(directory);
    }
}
