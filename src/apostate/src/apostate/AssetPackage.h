
#pragma once 

#include <string>

namespace apostate {
    class AssetPackage {
    public:
        std::string loadTextFile(const std::string &fileName) const;
    };
}
