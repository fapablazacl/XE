
#pragma once

#include "ModelLoader.h"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <vector>

class ModelLoaderAssimp : public ModelLoader {
public:
    ModelLoaderAssimp() = default;

    ~ModelLoaderAssimp() override = default;

    Model createModel(const std::string &sceneFilePath, Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location) override;

private:
    Model
    createModel(Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location, const aiScene &scene, const std::string &sceneFileParentPath) const;

    MeshNode createMeshNode(const aiNode &in) const;

    static std::vector<uint32_t> createMeshIndices(const aiNode &node) ;

    static std::vector<Mesh> createMeshArray(Renderer &renderer, const ShaderLocationMap &location, const aiScene &aiscene) ;


    Assimp::Importer importer;
};
