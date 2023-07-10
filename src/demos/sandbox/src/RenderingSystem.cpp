

#include "RenderingSystem.h"
#include "Asset_CGLTF.h"
#include "Assets.h"
#include "Scene.h"

#include <xe/XE.h>
#include <xe/graphics/GL.h>
#include <xe/graphics/Graphics.h>
#include <xe/graphics/PNG.h>
#include <xe/input/Input.h>
#include <xe/io/IO.h>
#include <xe/math/Math.h>
#include <xe/platform/glfw/WindowGLFW.h>

#include <fstream>
#include <iostream>
#include <string>

using XE::BinaryState;
using XE::Buffer;
using XE::BufferAccess;
using XE::BufferDescriptor;
using XE::BufferType;
using XE::BufferUsage;
using XE::ClearFlags;
using XE::DataType;
using XE::FileStreamSource;
using XE::GraphicsContext;
using XE::GraphicsDevice;
using XE::GraphicsDeviceGL;
using XE::ImageLoaderPNG;
using XE::InputManager;
using XE::KeyboardStatus;
using XE::KeyCode;
using XE::Matrix4;
using XE::PixelFormat;
using XE::Program;
using XE::ProgramDescriptor;
using XE::radians;
using XE::ShaderType;
using XE::Subset;
using XE::SubsetEnvelope;
using XE::Texture2D;
using XE::TVector;
using XE::Uniform;
using XE::UniformMatrix;
using XE::Vector2i;
using XE::Vector3;
using XE::Vector4;
using XE::WindowGLFW;

namespace Sandbox {
    void Camera::update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight) {
        // camera turning
        const auto cameraDirection = normalize(lookAt - position);
        float turnSpeed = 0.0f;
        if (turnLeft) {
            turnSpeed = -XE::radians(turnSpeedPerSecond * seconds);
        } else if (turnRight) {
            turnSpeed = XE::radians(turnSpeedPerSecond * seconds);
        }

        const auto cdt = mat4Rotation(turnSpeed, up) * Vector4(cameraDirection, 0.0f);

        lookAt = position + Vector3{cdt.X, cdt.Y, cdt.Z} * norm(cameraDirection);

        // camera movement
        const auto cameraSpeed = moveSpeedPerSecond * seconds;
        const auto cameraDisplacement = cameraDirection * cameraSpeed * seconds;
        // const auto cameraSide = normalize(cross(cameraDirection, up));

        if (moveForward) {
            position += cameraDisplacement;
            lookAt += cameraDisplacement;
        } else if (moveBackward) {
            position -= cameraDisplacement;
            lookAt -= cameraDisplacement;
        }
    }


    RenderingSystem::RenderingSystem(ILogger *logger) {
        mLogger = logger;

        sceneDescription = {{
            SceneModel("assets/models/cube.fbx")
        }};
    }


    void RenderingSystem::Initialize() {
        mLogger->log("Initializing");

        GraphicsContext::Descriptor descriptor;
        descriptor.backend = XE::GraphicsBackend::GL_41;


        m_window = WindowGLFW::create(descriptor, "XE.SandboxApp", {1024, 768}, false);
        mLogger->log("Created GLFW Window");

        m_graphicsDevice = std::make_unique<GraphicsDeviceGL>(m_window->getContext());
        mLogger->log("Created GraphicsDevice");

        m_inputManager = m_window->getInputManager();

        m_streamSource = std::make_unique<FileStreamSource>(".");

        initializeShaders();
        initializeGeometry();
    }

    void RenderingSystem::Update(const float seconds) {
        m_inputManager->poll();

        const KeyboardStatus keyboardStatus = m_inputManager->getKeyboardStatus();

        if (keyboardStatus.getState(KeyCode::KeyEsc) == BinaryState::Press) {
            m_shouldClose = true;
        }

        // update animation (rotation angle)
        m_angle += 60.0f * seconds;

        // update camera position
        const bool moveForward = keyboardStatus.getState(KeyCode::KeyUp) == BinaryState::Press;
        const bool moveBackward = keyboardStatus.getState(KeyCode::KeyDown) == BinaryState::Press;

        const bool turnLeft = keyboardStatus.getState(KeyCode::KeyLeft) == BinaryState::Press;
        const bool turnRight = keyboardStatus.getState(KeyCode::KeyRight) == BinaryState::Press;

        mCamera.update(seconds, moveForward, moveBackward, turnLeft, turnRight);
    }

    void RenderingSystem::Render() {
        m_graphicsDevice->beginFrame(ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);
        m_graphicsDevice->setProgram(m_program);

        Uniform textureUniform = {"texture0", DataType::Int32, XE::UniformDimension::D1, 1};
        int textureUnit = 0;

        m_graphicsDevice->applyUniform(&textureUniform, 1, reinterpret_cast<void *>(&textureUnit));
        m_graphicsDevice->setMaterial(m_material.get());

        renderScene();

        m_graphicsDevice->endFrame();
    }

    bool RenderingSystem::ShouldClose() const { return m_shouldClose; }

    void RenderingSystem::renderScene() {
        mAssetGLTF.visitDefaultScene([this](const XE::Matrix4 &matrix, const std::string &objectName) {
            renderMatrices(matrix);

            const GeoObject &object = mObjectsByNameMap[objectName];

            for (const auto &subset_N_envelope : object.subsets) {
                m_graphicsDevice->draw(subset_N_envelope.first, &subset_N_envelope.second, 1);
            }
        });
    }

    void RenderingSystem::renderObjects() {
        for (const auto &pair : mObjectsByNameMap) {
            for (const auto &subset_N_envelope : pair.second.subsets) {
                m_graphicsDevice->draw(subset_N_envelope.first, &subset_N_envelope.second, 1);
            }
        }
    }

    void RenderingSystem::renderMatrices(const XE::Matrix4 &) {
        const XE::UniformMatrix matrixLayout = {"m_mvp", XE::DataType::Float32, XE::UniformMatrixShape::R4C4, 1};

        const Matrix4 proj = XE::mat4Perspective(mCamera.fov, mCamera.aspectRatio, mCamera.znear, mCamera.zfar);
        const Matrix4 view = XE::mat4LookAtRH(mCamera.position, mCamera.lookAt, mCamera.up);
        const Matrix4 modelViewProj = XE::transpose(proj * view);

        m_graphicsDevice->applyUniform(&matrixLayout, 1, (const void *)&modelViewProj);
    }

    Texture2D *RenderingSystem::createColorTexture(const int width, const int height, const Vector4 &color) {
        const PixelFormat format = PixelFormat::R8G8B8A8;
        const Vector2i size = Vector2i{width, height};

        const PixelFormat sourceFormat = PixelFormat::R8G8B8A8;
        const DataType sourceDataType = DataType::UInt8;

        std::vector<TVector<std::uint8_t, 4>> pixels;

        pixels.resize(width * height);

        for (TVector<std::uint8_t, 4> &pixel : pixels) {
            pixel = (color * Vector4{255.0f}).cast<std::uint8_t>();
        }

        return m_graphicsDevice->createTexture2D(format, size, sourceFormat, sourceDataType, pixels.data());
    }

    Texture2D *RenderingSystem::createFileTexture(const std::string &filePath) {
        assert(m_streamSource);

        auto it = mTexturesByName.find(filePath);

        if (it != mTexturesByName.end()) {
            return it->second;
        }

        auto stream = m_streamSource->open(filePath);
        auto imagePtr = m_imageLoaderPNG.load(stream.get());

        Texture2D *texture = m_graphicsDevice->createTexture2D(PixelFormat::R8G8B8A8, imagePtr->getSize(), imagePtr->getFormat(), DataType::UInt8, imagePtr->getPointer());

        mTexturesByName.insert({filePath, texture});

        mLogger->log("Loaded texture map '" + filePath + "'");

        return texture;
    }

    SceneDescription RenderingSystem::createSceneDescription(int argc, char **argv) {
        assert(argc > 0);
        assert(argv);

        if (argc < 2) {
            throw std::runtime_error("Missing 3D model file in command line");
        }

        SceneDescription description;
        description.models.push_back(SceneModel(argv[1]));

        return description;
    }

    void RenderingSystem::initializeShaders() {
        // setup program shader
        const ProgramDescriptor programDescriptor = {
            {{ShaderType::Vertex, loadTextFile("media/shaders/simple/vertex.glsl")}, {ShaderType::Fragment, loadTextFile("media/shaders/simple/fragment.glsl")}}};

        m_program = m_graphicsDevice->createProgram(programDescriptor);

        mLogger->log("Created shader program");
    }

    std::string RenderingSystem::loadTextFile(const std::string &path) const {
        std::fstream fs;
        fs.open(path.c_str(), std::ios_base::in);

        if (!fs.is_open()) {
            throw std::runtime_error("Couldn't load text file '" + path + "'. Check that the file exists.");
        }

        std::string content;
        std::string line;

        while (!fs.eof()) {
            std::getline(fs, line);

            content += line;

            if (!fs.eof()) {
                content += "\n";
            }
        }

        mLogger->log("Loaded text file content " + path);

        return content;
    }

    void RenderingSystem::initializeGeometry() {
        if (sceneDescription.models.size() == 0) {
            std::cout << "The models arrays is empty" << std::endl;
            return;
        }

        const std::string filePath = sceneDescription.models[0].filePath;

        mObjectsByNameMap = loadGeoObject(filePath);

        m_texture = createFileTexture("media/materials/Tiles_Azulejos_004_SD/Tiles_Azulejos_004_COLOR.png");
        m_material = std::make_unique<XE::Material>();
        m_material->layers[0].texture = m_texture;
        m_material->layerCount = 1;
        m_material->renderState.depthTest = true;
    }

    std::map<std::string, GeoObject> RenderingSystem::loadGeoObject(const std::string &sceneFilePath) {
        std::map<std::string, GeoObject> objectsByName;

        mAssetGLTF.load(sceneFilePath);

        auto meshes = mAssetGLTF.getMeshes();

        for (const auto &mesh : meshes) {
            GeoObject geoObject = {};

            for (const auto &primitive : mesh.primitives) {
                geoObject.subsets.push_back(createSubset(primitive));
            }

            objectsByName[mesh.name] = std::move(geoObject);
        }

        mLogger->log("Loaded scene model '" + sceneFilePath + "'");

        return objectsByName;
    }

    std::pair<Subset *, SubsetEnvelope> RenderingSystem::createSubset(const MeshPrimitive &meshPrimitive) {
        // create the vertex buffer
        BufferDescriptor coordBufferDescriptor = {BufferType::Vertex, BufferUsage::Copy, BufferAccess::Static, meshPrimitive.coords.size() * sizeof(Vector3),
                                                  (const void *)meshPrimitive.coords.data()};

        XE::Buffer *coordBuffer = m_graphicsDevice->createBuffer(coordBufferDescriptor);

        BufferDescriptor colorBufferDescriptor = {BufferType::Vertex, BufferUsage::Copy, BufferAccess::Static, meshPrimitive.colors.size() * sizeof(Vector4),
                                                  (const void *)meshPrimitive.colors.data()};

        XE::Buffer *colorBuffer = m_graphicsDevice->createBuffer(colorBufferDescriptor);

        BufferDescriptor normalBufferDescriptor = {BufferType::Vertex, BufferUsage::Copy, BufferAccess::Static, meshPrimitive.normals.size() * sizeof(Vector3),
                                                   (const void *)meshPrimitive.normals.data()};

        XE::Buffer *normalBuffer = m_graphicsDevice->createBuffer(normalBufferDescriptor);

        BufferDescriptor texCoordBufferDescriptor = {BufferType::Vertex, BufferUsage::Copy, BufferAccess::Static, meshPrimitive.texCoords.size() * sizeof(Vector3),
                                                     (const void *)meshPrimitive.texCoords.data()};

        XE::Buffer *texCoordBuffer = m_graphicsDevice->createBuffer(texCoordBufferDescriptor);

        // create the index buffer
        BufferDescriptor indexBufferDescriptor = {BufferType::Index, BufferUsage::Copy, BufferAccess::Static, meshPrimitive.indices.size() * sizeof(int),
                                                  (const void *)meshPrimitive.indices.data()};

        XE::Buffer *indexBuffer = m_graphicsDevice->createBuffer(indexBufferDescriptor);

        // create the geometry subset
        XE::SubsetDescriptor subsetDescriptor;

        subsetDescriptor.attribs = {XE::SubsetVertexAttrib{0, DataType::Float32, 3}, XE::SubsetVertexAttrib{1, DataType::Float32, 4},
                                    XE::SubsetVertexAttrib{2, DataType::Float32, 3}, XE::SubsetVertexAttrib{3, DataType::Float32, 2}};
        subsetDescriptor.indexBuffer = indexBuffer;
        subsetDescriptor.buffers = {coordBuffer, colorBuffer, normalBuffer, texCoordBuffer};

        return {m_graphicsDevice->createSubset(subsetDescriptor), meshPrimitive.getEnvelope()};
    }
} // namespace Sandbox
