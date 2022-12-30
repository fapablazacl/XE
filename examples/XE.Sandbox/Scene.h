
#pragma once 

#include <string>
#include <vector>

struct SceneModel {
    std::string filePath;

    SceneModel() {}

    explicit SceneModel(const std::string &filePath) : filePath(filePath) {}
};


/**
 * @brief A description for a 3D Scene. This will needed to be further processed to be renderable and usable
 */
struct SceneDescription {
    //! Array of File Paths, for the models to be 
    std::vector<SceneModel> models;
};
