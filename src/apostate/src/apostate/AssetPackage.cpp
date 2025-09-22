
#include "AssetPackage.h"

#include <cassert>
#include <fstream>
#include <ios>
#include <string>

namespace apostate {
    std::string AssetPackage::loadTextFile(const std::string &fileName) {
        assert(!fileName.empty());

        std::fstream fs;

        fs.open(fileName.c_str(), std::ios::in);
        assert(fs.is_open());

        std::string content;
        std::string line;

        while (fs.good()) {
            std::getline(fs, line);
            line += "\n";
            content += line;
        }

        return content;
    }
} // namespace apostate
