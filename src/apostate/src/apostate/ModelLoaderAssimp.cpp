
#include "ModelLoaderAssimp.h"

#include "MiscUtils.h"
#include "Model.h"
#include "xe/Logger.h"

#include <filesystem>
#include <glm/fwd.hpp>
#include <iostream>
#include <optional>
#include <regex>
#include <stdexcept>

namespace fs = std::filesystem;

inline static glm::mat4 Assimp2Glm(const aiMatrix4x4 &from) {
    return {
        (double)from.a1,
        (double)from.b1,
        (double)from.c1,
        (double)from.d1,
        (double)from.a2,
        (double)from.b2,
        (double)from.c2,
        (double)from.d2,
        (double)from.a3,
        (double)from.b3,
        (double)from.c3,
        (double)from.d3,
        (double)from.a4,
        (double)from.b4,
        (double)from.c4,
        (double)from.d4
    };
}

/*
inline glm::mat4 computeNodeTransformation(const aiNode* node) {
    auto transformation = glm::identity<glm::mat4>();

    while (node != nullptr) {
        transformation = Assimp2Glm(node->mTransformation) * transformation;

        node = node->mParent;
    }

    return transformation;
}
*/

inline std::string to_str(const aiTextureType type) {
    switch (type) {
    case aiTextureType_AMBIENT:
        return "aiTextureType_AMBIENT";
    case aiTextureType_DIFFUSE:
        return "aiTextureType_DIFFUSE";
    case aiTextureType_SPECULAR:
        return "aiTextureType_SPECULAR";
    case aiTextureType_HEIGHT:
        return "aiTextureType_HEIGHT";
    case aiTextureType_EMISSIVE:
        return "aiTextureType_EMISSIVE";
    case aiTextureType_NORMALS:
        return "aiTextureType_NORMALS";
    case aiTextureType_SHININESS:
        return "aiTextureType_SHININESS";
    case aiTextureType_OPACITY:
        return "aiTextureType_OPACITY";
    case aiTextureType_DISPLACEMENT:
        return "aiTextureType_DISPLACEMENT";
    case aiTextureType_LIGHTMAP:
        return "aiTextureType_LIGHTMAP";
    case aiTextureType_REFLECTION:
        return "aiTextureType_REFLECTION";
    case aiTextureType_BASE_COLOR:
        return "aiTextureType_BASE_COLOR";
    case aiTextureType_NORMAL_CAMERA:
        return "aiTextureType_NORMAL_CAMERA";
    case aiTextureType_EMISSION_COLOR:
        return "aiTextureType_EMISSION_COLOR";
    case aiTextureType_METALNESS:
        return "aiTextureType_METALNESS";
    case aiTextureType_DIFFUSE_ROUGHNESS:
        return "aiTextureType_DIFFUSE_ROUGHNESS";
    case aiTextureType_AMBIENT_OCCLUSION:
        return "aiTextureType_AMBIENT_OCCLUSION";
    case aiTextureType_UNKNOWN:
        return "aiTextureType_UNKNOWN";
    default:
        return "Uknnown aiTextureType enumeration value: " + std::to_string(static_cast<int>(type));
    }
}

/**
 * @brief Exports the current AssImporter scene to the specified text output stream, in JSON formatting.
 *
 * It's needed because we need to review the internal structure of the scene created by AssImp,
 * for debugging activities while working in the Renderer.
 */
/*
void exportSceneStructure(std::ostream &os, const std::string &sceneFileName, const aiScene *scene) {
    nlohmann::json json;

    json["filename"] = sceneFileName;

    // meshes
    auto meshes = nlohmann::json::array();
    for (size_t i = 0; i<scene->mNumMeshes; i++) {
        meshes.push_back(scene->mMeshes[i]->mName.C_Str());
    }
    json["meshes"] = meshes;

    // materials
    auto materials = nlohmann::json::array();
    for (size_t i = 0; i<scene->mNumMaterials; i++) {
        auto material = nlohmann::json::object();
        auto aimaterial = scene->mMaterials[i];

        material["name"] = aimaterial->GetName().C_Str();
        materials.push_back(material);
    }
    json["materials"] = materials;

    os << json;
}
*/

/*
inline static aiMatrix4x4 Glm2Assimp(const glm::mat4& from) {
    return aiMatrix4x4(
        from[0][0], from[1][0], from[2][0], from[3][0],
        from[0][1], from[1][1], from[2][1], from[3][1],
        from[0][2], from[1][2], from[2][2], from[3][2],
        from[0][3], from[1][3], from[2][3], from[3][3]
    );
}
*/

Model ModelLoaderAssimp::createModel(const std::string &sceneFilePath, Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location) {
    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // propably to request more postprocessing than we do in this example.
    const auto flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals |
                       // aiProcess_CalcTangentSpace |
                       // aiProcess_SortByPType |
                       aiProcess_GlobalScale | aiProcess_ValidateDataStructure;

    const std::string sceneFileParentPath = parent_path(sceneFilePath);
    const aiScene *scene = importer.ReadFile(sceneFilePath, flags);

    // If the import failed, report it
    if (!scene) {
        XE_LOG_ERROR("Failed to load scene at {}. Assimp error: {}\n", sceneFilePath, importer.GetErrorString());
        throw std::runtime_error(importer.GetErrorString());
    }

    if (!scene->HasMeshes()) {
        throw std::runtime_error("scene doesn't have meshes");
    }

    if (scene->HasAnimations()) {
        std::cout << "scene has animations: " << scene->mNumAnimations << '\n';

        for (int i = 0; i < scene->mNumAnimations; i++) {
            std::cout << "    " << " animation " << (i + 1) << scene->mAnimations[i]->mName.C_Str() << '\n';
        }
    }

    return createModel(renderer, textureRepository, location, *scene, sceneFileParentPath);
}

MeshNode ModelLoaderAssimp::createMeshNode(const aiNode &in) const {
    MeshNode out;

    out.transform = Assimp2Glm(in.mTransformation);
    out.meshIndices = createMeshIndices(in);

    for (size_t i = 0; i < in.mNumChildren; i++) {
        MeshNode child = createMeshNode(*in.mChildren[i]);
        out.children.push_back(child);
    }

    return out;
}

std::vector<uint32_t> ModelLoaderAssimp::createMeshIndices(const aiNode &node) const {
    std::vector<uint32_t> indices;

    indices.reserve(node.mNumMeshes);
    for (unsigned int i = 0; i < node.mNumMeshes; i++) {
        indices.push_back(node.mMeshes[i]);
    }

    return indices;
}

Mesh createMeshVAO(const ShaderLocationMap &location, Renderer &renderer, const aiMesh *mesh) {
    if (!mesh) {
        return {};
    }

    MeshData data;

    data.materialIndex = mesh->mMaterialIndex;

    data.vertexCoord = MeshAttribute{mesh->mVertices, mesh->mNumVertices * sizeof(aiVector3D)};

    if (mesh->HasNormals()) {
        data.vertexNormal = MeshAttribute{mesh->mNormals, mesh->mNumVertices * sizeof(aiVector3D)};
    }

    std::vector<glm::vec2> texCoords;

    if (mesh->mTextureCoords[0]) {
        assert(mesh->mNumUVComponents[0] == 2);

        texCoords.resize(mesh->mNumVertices);

        for (size_t i = 0; i < texCoords.size(); i++) {
            const auto &tc = mesh->mTextureCoords[0][i];

            texCoords[i] = glm::vec2{tc.x, tc.y};
        }

        data.vertexTexCoord = MeshAttribute{texCoords.data(), texCoords.size() * sizeof(glm::vec2)};
    }

    std::vector<unsigned int> indices;

    if (mesh->HasFaces()) {
        indices.reserve(mesh->mNumFaces);

        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            const aiFace &face = mesh->mFaces[j];

            assert("This function requires the aiTriangulate postprocessing flag" && face.mNumIndices == 3);

            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        data.indices = MeshAttribute{indices.data(), indices.size() * sizeof(unsigned int)};
        data.indexCount = indices.size();
    }

    return renderer.createMeshVAO(location, data);
}

std::vector<Mesh> ModelLoaderAssimp::createMeshArray(Renderer &renderer, const ShaderLocationMap &location, const aiScene &aiscene) const {
    std::vector<Mesh> meshes;
    meshes.resize(aiscene.mNumMeshes);

    for (unsigned i = 0; i < aiscene.mNumMeshes; i++) {
        meshes[i] = createMeshVAO(location, renderer, aiscene.mMeshes[i]);
    }

    return meshes;
}

std::vector<Mesh> createMeshArray(const ShaderLocationMap &location, Renderer &renderer, const aiScene *aiscene) {
    if (!aiscene) {
        return {};
    }

    std::vector<Mesh> meshes;
    meshes.resize(aiscene->mNumMeshes);

    for (unsigned i = 0; i < aiscene->mNumMeshes; i++) {
        meshes[i] = createMeshVAO(location, renderer, aiscene->mMeshes[i]);
    }

    return meshes;
}

std::vector<GLuint> createTextureArray(const aiScene *scene, Renderer &renderer, TextureRepository &textureRepository) {
    assert(scene);

    if (!scene->HasTextures()) {
        std::cout << "Scene \"" << scene->mName.C_Str() << "\" doesn't have global textures" << '\n';
        return {};
    }

    std::cout << "Loading " << scene->mNumTextures << " textures for scene \"" << scene->mName.C_Str() << "\"" << '\n';

    std::vector<GLuint> textures;
    textures.resize(scene->mNumTextures);

    for (size_t ti = 0; ti < scene->mNumTextures; ti++) {
        const unsigned width = scene->mTextures[ti]->mWidth;
        const unsigned height = scene->mTextures[ti]->mHeight;

        const void *data = scene->mTextures[ti]->pcData;
        const auto formatHint = scene->mTextures[ti]->achFormatHint[0] & 0x01;
        const auto name = "\"" + std::string(scene->mTextures[ti]->mFilename.C_Str()) + "\"";

        if (height == 0) {
            const std::string imageTypeHint = scene->mTextures[ti]->achFormatHint;
            XE_LOG_INFO("Going to load compressed texture map {} with {} format from buffer {}\n", name, imageTypeHint, data);
            textures[ti] = textureRepository.createTexture(renderer, imageTypeHint, width, data);
        } else {
            XE_LOG_INFO("Loading texture map {} with size {}x{}", name, width, height);

            const GLuint wrap = (formatHint) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
            textures[ti] = renderer.createTexture(GL_RGBA8, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data, wrap, wrap);
        }
    }

    return textures;
}

template <typename T> T extract(std::optional<T> value, const T defaultValue) {
    return value.has_value() ? value.value() : defaultValue;
}

glm::vec4 makeVector4(const aiColor3D &color, const float alpha) {
    return glm::vec4{color.r, color.g, color.b, alpha};
}

MaterialImportedProperties extractProperties(const aiMaterial &material) {
    MaterialImportedProperties props;
    aiColor3D color;

    if (material.Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        props.ambient = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_AMBIENT property mapped: " << glm::to_string(*props.ambient) << '\n';
    }

    if (material.Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        props.diffuse = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_DIFFUSE property mapped: " << glm::to_string(*props.diffuse) << '\n';
    }

    if (material.Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        props.specular = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_SPECULAR property mapped: " << glm::to_string(*props.specular) << '\n';
    }

    if (material.Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
        props.emissive = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_EMISSIVE property mapped: " << glm::to_string(*props.emissive) << '\n';
    }

    if (material.Get(AI_MATKEY_COLOR_TRANSPARENT, color) == AI_SUCCESS) {
        props.transparent = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_TRANSPARENT property mapped: " << glm::to_string(*props.transparent) << '\n';
    }

    if (material.Get(AI_MATKEY_COLOR_REFLECTIVE, color) == AI_SUCCESS) {
        props.reflective = makeVector4(color, 1.0f);
        std::cout << "    " << "AI_MATKEY_COLOR_REFLECTIVE property mapped: " << glm::to_string(*props.reflective) << '\n';
    }

    return props;
}

void setupTextureMap(Material &material, const GLuint textureMap, const aiTextureType textureType) {
    switch (textureType) {
    case aiTextureType_AMBIENT:
        material.ambient.textureMap = textureMap;
        break;

    case aiTextureType_DIFFUSE:
        material.diffuse.textureMap = textureMap;
        break;

    case aiTextureType_SPECULAR:
        material.specular.textureMap = textureMap;
        break;

    case aiTextureType_EMISSIVE:
        material.emissive.textureMap = textureMap;
        break;

    default:
        XE_LOG_WARNING("Missing case for texture type {}\n", static_cast<int>(textureType));
    }
}

void searchFilesImpl(std::vector<fs::path> &files, const fs::path &directory, const std::regex &pattern) {
    for (const fs::directory_entry &entry : fs::directory_iterator(directory)) {
        if (fs::is_directory(entry.status())) {
            searchFilesImpl(files, entry.path(), pattern);
        } else if (fs::is_regular_file(entry.status())) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, pattern)) {
                files.push_back(entry.path());
            }
        }
    }
}

std::vector<fs::path> searchFiles(const fs::path &directory, const std::regex &pattern) {
    std::vector<fs::path> files;

    searchFilesImpl(files, directory, pattern);

    return files;
}

std::optional<fs::path> locate_texture(const fs::path &parentPath, const fs::path &textureFilePath) {
    const std::vector<fs::path> combinations = {
        parentPath / textureFilePath,
        parentPath / textureFilePath.filename(),
        parentPath.parent_path().parent_path() / textureFilePath,
        parentPath.parent_path().parent_path() / textureFilePath.filename(),
        parentPath.parent_path().parent_path() / "textures" / textureFilePath,
        parentPath.parent_path().parent_path() / "textures" / textureFilePath.filename(),
    };

    for (const fs::path &path : combinations) {
        std::cout << "Trying " << path.string() << '\n';

        if (fs::exists(path)) {
            return path;
        }
    }

    const std::string patternStr = textureFilePath.stem().string() + "\\..*";
    const std::regex pattern{patternStr};
    const fs::path searchPath = parentPath.parent_path().parent_path();

    std::cout << "Trying to locate texture in " << searchPath << " with alias " << patternStr << '\n';

    const std::vector<fs::path> foundFiles = searchFiles(searchPath, pattern);

    std::cout << "Found " << foundFiles.size() << " matches" << '\n';

    if (foundFiles.empty()) {
        return {};
    }

    return foundFiles[0];
}

Material
createMaterial(const std::string &parentPath, Renderer &renderer, TextureRepository &textureRepository, const aiMaterial *aimaterial, const std::vector<GLuint> &textures) {
    if (!aimaterial) {
        return {};
    }

    XE_LOG_INFO("Creating material {}\n", aimaterial->GetName().C_Str());

    Material material;

    // extract material colors
    const MaterialImportedProperties props = extractProperties(*aimaterial);
    material.ambient.color = extract(props.ambient, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    material.diffuse.color = extract(props.diffuse, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    material.specular.color = extract(props.specular, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});
    material.emissive.color = extract(props.emissive, glm::vec4{0.0f, 0.0f, 0.0f, 1.0f});

    // extract material textures
    std::map<aiTextureType, std::string> textureMap{
        {aiTextureType_AMBIENT, ""},
        {aiTextureType_DIFFUSE, ""},
        {aiTextureType_SPECULAR, ""},
        {aiTextureType_HEIGHT, ""},
        {aiTextureType_EMISSIVE, ""},
        {aiTextureType_NORMALS, ""},
        {aiTextureType_SHININESS, ""},
        {aiTextureType_OPACITY, ""},
        {aiTextureType_DISPLACEMENT, ""},
        {aiTextureType_LIGHTMAP, ""},
        {aiTextureType_REFLECTION, ""},
        {aiTextureType_BASE_COLOR, ""},
        {aiTextureType_NORMAL_CAMERA, ""},
        {aiTextureType_EMISSION_COLOR, ""},
        {aiTextureType_METALNESS, ""},
        {aiTextureType_DIFFUSE_ROUGHNESS, ""},
        {aiTextureType_AMBIENT_OCCLUSION, ""},
        {aiTextureType_UNKNOWN, ""},
    };

    for (auto &pair : textureMap) {
        const aiTextureType textureType = pair.first;

        aiString textureFileName;
        aimaterial->GetTexture(textureType, 0, &textureFileName);

        if (textureFileName.length == 0) {
            continue;
        }

        const std::string fileName = textureFileName.C_Str();

        std::cout << "    " << "Texture type: " << to_str(textureType) << ", has this filename: \"" << fileName << "\"" << '\n';
        std::string filePath = join(split(fileName, "\\"), "/");

        if (filePath[0] == '*') {
            std::cout << "Texture embedded directly into the scene" << '\n';

            const std::string indexPart = fileName.substr(1, fileName.size() - 1);
            const auto index = static_cast<size_t>(std::atoi(indexPart.c_str()));

            if (index < textures.size()) {
                std::cout << "    " << "Linking texture index " << index << " in this material." << '\n';

                setupTextureMap(material, textures[index], textureType);
                continue;
            } else {
                std::cout << "    " << "Can't link texture index " << index << " in this material. Scene Textures just have " << (textures.size() + 1) << " elements" << '\n';
            }
        } else if (filePath[0] == '/') {
            if (!can_be_opened(filePath)) {
                const std::string textureParentPath = parent_path(filePath);

                filePath = replace_all(filePath, parent_path(filePath), parentPath);
            }
        } else {
            std::cout << "Locating texture " << filePath << " in tree of " << parentPath << '\n';
            const auto path = locate_texture(parentPath, filePath);

            if (path.has_value()) {
                std::cout << "Texture " << filePath << " located at " << path.value().string() << '\n';

                filePath = path.value().string();
            } else {
                std::cout << "Texture " << filePath << " couldn't be located" << '\n';
            }
        }

        if (can_be_opened(filePath)) {
            pair.second = filePath;
            std::cout << "    " << "Normalizing path for texture: " << to_str(textureType) << " = " << fileName << " -> " << filePath << '\n';

            setupTextureMap(material, textureRepository.getOrCreate(filePath, renderer), textureType);
        } else {
            std::cout << "    " << "Texture filepath normalization failed, because it cannot be opened. The generated filepath was: " << filePath << '\n';
        }
    }

    return material;
}

std::vector<Material>
createMaterialArray(const std::string &parentPath, Renderer &renderer, TextureRepository &textureRepository, const aiScene *aiscene, const std::vector<GLuint> &textures) {
    std::vector<Material> materials;

    materials.resize(aiscene->mNumMaterials);

    for (unsigned int i = 0; i < aiscene->mNumMaterials; i++) {
        materials[i] = createMaterial(parentPath, renderer, textureRepository, aiscene->mMaterials[i], textures);
    }

    return materials;
}

Model ModelLoaderAssimp::createModel(
    Renderer &renderer, TextureRepository &textureRepository, const ShaderLocationMap &location, const aiScene &scene, const std::string &sceneFileParentPath
) const {
    Model model;

    model.meshes = createMeshArray(renderer, location, scene);
    model.rootNode = createMeshNode(*scene.mRootNode);
    model.textures = createTextureArray(&scene, renderer, textureRepository);
    model.materials = createMaterialArray(sceneFileParentPath, renderer, textureRepository, &scene, model.textures);

    return model;
}
