
#pragma once

#include "ModelLoader.h"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <vector>

class ModelLoaderAssimp : public ModelLoader {
public:
    ModelLoaderAssimp() = default;

    virtual ~ModelLoaderAssimp() = default;

    Model createModel(const std::string &fileName, Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location) override;

private:
    Model
    createModel(Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location, const aiScene &scene, const std::string &sceneFileParentPath) const;

    MeshNode createMeshNode(const aiNode &in) const;

    std::vector<uint32_t> createMeshIndices(const aiNode &node) const;

    std::vector<Mesh> createMeshArray(Renderer &renderer, const ShaderLocationMap &location, const aiScene &aiscene) const;

private:
    Assimp::Importer importer;
};
