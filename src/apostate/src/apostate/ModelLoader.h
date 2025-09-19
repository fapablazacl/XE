
#pragma once

#include <string>

struct Mesh;
struct MeshNode;
struct Model;
struct ShaderLocationMap;

class Renderer;
class TextureRepository;

class ModelLoader {
public:
    virtual ~ModelLoader() {
    }

    virtual Model createModel(const std::string &fileName, Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location) = 0;
};
