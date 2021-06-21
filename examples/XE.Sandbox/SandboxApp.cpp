

#include "SandboxApp.hpp"
#include "Assets.hpp"
#include "Asset_CGLTF.h"

#include <XE/XE.h>
#include <XE/Math.h>
#include <XE/Input.h>
#include <XE/IO.h>
#include <XE/Graphics.h>
#include <XE/Graphics/GL.h>
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>
#include <XE/Graphics/PNG.h>

#include <string>
#include <iostream>
#include <fstream>

using XE::Vector3f;
using XE::Vector4f;
using XE::Vector2i;
using XE::radians;
using XE::Matrix4f;
using XE::WindowGLFW;
using XE::GraphicsDeviceGL;
using XE::Subset;
using XE::SubsetEnvelope;
using XE::ContextDescriptorGL;
using XE::KeyboardStatus;
using XE::KeyCode;
using XE::FileStreamSource;
using XE::UniformMatrix;
using XE::Uniform;
using XE::BinaryState;
using XE::DataType;
using XE::PixelFormat;
using XE::GraphicsDevice;
using XE::ImageLoaderPNG;
using XE::InputManager;
using XE::ProgramDescriptor;
using XE::Texture2D;
using XE::Program;
using XE::Vector;
using XE::ClearFlags;
using XE::ShaderType;
using XE::Buffer;
using XE::BufferDescriptor;
using XE::BufferType;
using XE::BufferUsage;
using XE::BufferAccess;
using XE::SubsetDescriptor;


namespace Sandbox {
    struct Camera {
        Vector3f position = {0.0f, 0.0f, 15.0f};
        Vector3f lookAt = {0.0f, 0.0f, 0.0f};
        Vector3f up = {0.0f, 1.0f, 0.0f};

        float fov = radians(60.0f);
        float aspectRatio = 640.0f / 480.0f;
        float znear = 0.1f;
        float zfar = 1000.0f;
        
        void update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight) {
            // camera turning
            const auto cameraDirection = normalize(lookAt - position);
            float turnSpeed = 0.0f;
            if (turnLeft) {
                turnSpeed = -radians(1.25f * seconds);
            } else if (turnRight) {
                turnSpeed = radians(1.25f * seconds);
            }

            const auto cdt = Matrix4f::createRotation(turnSpeed, up) * Vector4f(cameraDirection, 0.0f);
            
            lookAt = position + Vector3f{cdt.X, cdt.Y, cdt.Z} * norm(cameraDirection);

            // camera movement
            const auto cameraSpeed = 0.025f * seconds;
            const auto cameraDisplacement = cameraDirection * cameraSpeed * seconds;
            const auto cameraSide = normalize(cross(cameraDirection, up));
            
            if (moveForward) {
                position += cameraDisplacement;
                lookAt += cameraDisplacement;
            } else if (moveBackward) {
                position -= cameraDisplacement;
                lookAt -= cameraDisplacement;
            }
        }
    };


    struct GeoObject {
        std::vector<std::pair<Subset*, SubsetEnvelope>> subsets;
    };

    
    class SandboxApp : public Application {
    public:
        explicit SandboxApp() {}
        
        virtual ~SandboxApp() {}

        void Initialize() override {
            std::cout << "Initializing Engine ..." << std::endl;

            m_window = WindowGLFW::create (
                ContextDescriptorGL::defaultGL4(), 
                "XE.SandboxApp",
                {1024, 768},
                false
            );

            m_graphicsDevice = std::make_unique<GraphicsDeviceGL>(m_window->getContext());
            m_inputManager = m_window->getInputManager();

            std::cout << "Loading assets ..." << std::endl;
            m_streamSource = std::make_unique<FileStreamSource>(".");
            
            initializeShaders();
            initializeGeometry();
        }

        
        void Update(const float seconds) override {
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

        
        void renderMatrices(const XE::Matrix4f &model = XE::Matrix4f::createIdentity()) {
            const UniformMatrix matrixLayout = { "m_mvp", DataType::Float32, 4, 4, 1 };
            
            const Matrix4f modelViewProj = transpose (
                Matrix4f::createPerspective(mCamera.fov, mCamera.aspectRatio, mCamera.znear, mCamera.zfar) *
                Matrix4f::createLookAt(mCamera.position, mCamera.lookAt, mCamera.up) *
                Matrix4f::createRotationY(radians(m_angle)) 
            );

            m_graphicsDevice->applyUniform(&matrixLayout, 1, (const std::byte*)&modelViewProj);
        }

        
        Texture2D* createColorTexture(const int width, const int height, const Vector4f &color) {
            const auto format = PixelFormat::R8G8B8A8;
            const auto size = Vector2i{width, height};

            const auto sourceFormat = PixelFormat::R8G8B8A8;
            const auto sourceDataType = DataType::UInt8;

            std::vector<Vector<std::uint8_t, 4>> pixels;

            pixels.resize(width * height);

            for (auto &pixel : pixels) {
                pixel = (color * Vector4f{255.0f}).cast<std::uint8_t>();
            }

            return m_graphicsDevice->createTexture2D(format, size, sourceFormat, sourceDataType, pixels.data());
        }
        

        Texture2D* createFileTexture(const std::string &filePath) {
            assert(m_streamSource);
            
            auto it = mTexturesByName.find(filePath);
            
            if (it != mTexturesByName.end()) {
                return it->second;
            }
            
            std::cout   << "SandboxApp::CreateFileTexture: Loading texture from file "
                        << filePath << " ..." << std::endl;
            
            auto stream = m_streamSource->open(filePath);

            auto imagePtr = m_imageLoaderPNG.load(stream.get());

            Texture2D *texture = m_graphicsDevice->createTexture2D (
                PixelFormat::R8G8B8A8,
                imagePtr->getSize(),
                imagePtr->getFormat(),
                DataType::UInt8,
                imagePtr->getPointer()
            );
            
            mTexturesByName.insert({filePath, texture});
            
            return texture;
        }

        
        void Render() override {
            m_graphicsDevice->beginFrame(ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);
            m_graphicsDevice->setProgram(m_program);

            Uniform textureUniform = { "texture0", DataType::Int32, 1, 1 };
            int textureUnit = 0;

            m_graphicsDevice->applyUniform(&textureUniform, 1, reinterpret_cast<std::byte*>(&textureUnit));

            m_graphicsDevice->setMaterial(m_material.get());

            renderScene();

            m_graphicsDevice->endFrame();
        }

        bool ShouldClose() const override {
            return m_shouldClose;
        }

        
    private:
        void renderScene() {
            mAssetGLTF.visitDefaultScene([this](const XE::Matrix4f &matrix, const std::string &objectName) {
                renderMatrices(matrix);
                
                const GeoObject &object = mObjectsByNameMap[objectName];
                
                for (const auto &subset_N_envelope : object.subsets) {
                    m_graphicsDevice->draw(
                       subset_N_envelope.first,
                       &subset_N_envelope.second, 1);
                }
            });
        }
        
        
        void renderObjects() {
            for (const auto &pair : mObjectsByNameMap) {
                for (const auto &subset_N_envelope : pair.second.subsets) {
                    m_graphicsDevice->draw(
                       subset_N_envelope.first,
                       &subset_N_envelope.second, 1);
                }
            }
        }
        
        void initializeShaders() {
            // setup program shader
            const ProgramDescriptor programDescriptor = {
                {
                    {ShaderType::Vertex, loadTextFile("media/shaders/simple/vertex.glsl")},
                    {ShaderType::Fragment, loadTextFile("media/shaders/simple/fragment.glsl")}
                }
            };

            m_program = m_graphicsDevice->createProgram(programDescriptor);
        }

        
        std::string loadTextFile(const std::string &path) const {
            std::fstream fs;
            fs.open(path.c_str(), std::ios_base::in);

            if (!fs.is_open()) {
                throw std::runtime_error("Shared source file wasn't found");
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

            std::cout << content << std::endl;

            return content;
        }
        

        void initializeGeometry() {
            mObjectsByNameMap = loadGeoObject("media/models/spaceship_corridorhallway/scene.gltf");
            
            // mGeoObject = createGeoObject(Sandbox::Assets::getSquareMeshPrimitive());
            
            // m_texture = createColorTexture(256, 256, {1.0f, 0.0f, 0.0f, 1.0f});
            m_texture = createFileTexture("media/materials/Tiles_Azulejos_004_SD/Tiles_Azulejos_004_COLOR.png");

            m_material = std::make_unique<XE::Material>();
            m_material->layers[0].texture = m_texture;
            m_material->layerCount = 1;
            m_material->renderState.depthTest = true;
        }
        
        
        std::map<std::string, GeoObject> loadGeoObject(const std::string &sceneFilePath) {
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
            
            return objectsByName;
        }
        
        
        std::pair<Subset*, SubsetEnvelope> createSubset(const MeshPrimitive &meshPrimitive) {
            // create the vertex buffer
            BufferDescriptor coordBufferDescriptor = {
                BufferType::Vertex,
                BufferUsage::Copy,
                BufferAccess::Static,
                (int)meshPrimitive.coords.size() * (int)sizeof(Vector3f),
                (const std::byte*) meshPrimitive.coords.data()
            };
            
            auto coordBuffer = m_graphicsDevice->createBuffer(coordBufferDescriptor);

            BufferDescriptor colorBufferDescriptor = {
                BufferType::Vertex,
                BufferUsage::Copy,
                BufferAccess::Static,
                (int)meshPrimitive.colors.size() * (int)sizeof(Vector4f),
                (const std::byte*) meshPrimitive.colors.data()
            };

            auto colorBuffer = m_graphicsDevice->createBuffer(colorBufferDescriptor);

            BufferDescriptor normalBufferDescriptor = {
                BufferType::Vertex,
                BufferUsage::Copy,
                BufferAccess::Static,
                (int)meshPrimitive.normals.size() * (int)sizeof(Vector3f),
                (const std::byte*) meshPrimitive.normals.data()
            };

            auto normalBuffer = m_graphicsDevice->createBuffer(normalBufferDescriptor);

            BufferDescriptor texCoordBufferDescriptor = {
                BufferType::Vertex,
                BufferUsage::Copy,
                BufferAccess::Static,
                (int)meshPrimitive.texCoords.size() * (int)sizeof(Vector3f),
                (const std::byte*) meshPrimitive.texCoords.data()
            };

            auto texCoordBuffer = m_graphicsDevice->createBuffer(texCoordBufferDescriptor);

            // create the index buffer
            BufferDescriptor indexBufferDescriptor = {
                BufferType::Index,
                BufferUsage::Copy,
                BufferAccess::Static,
                (int)meshPrimitive.indices.size() * (int)sizeof(int),
                (const std::byte*) meshPrimitive.indices.data()
            };

            auto indexBuffer = m_graphicsDevice->createBuffer(indexBufferDescriptor);

            // create the geometry subset
            SubsetDescriptor subsetDescriptor;
        
            subsetDescriptor.attributes = {
                {"vertCoord", DataType::Float32, 3},
                {"vertColor", DataType::Float32, 4},
                {"vertNormal", DataType::Float32, 3},
                {"vertTexCoord", DataType::Float32, 2}
            };

            subsetDescriptor.indexType = DataType::UInt32;

            std::vector<Buffer*> buffers = {coordBuffer, colorBuffer, normalBuffer, texCoordBuffer};

            std::map<std::string, int> bufferMapping = {
                {"vertCoord", 0}, {"vertColor", 1}, {"vertNormal", 2}, {"vertTexCoord", 3}
            };

            return {
                m_graphicsDevice->createSubset(subsetDescriptor, buffers, bufferMapping, indexBuffer),
                meshPrimitive.getEnvelope()
            };
        }
        
    private:
        std::unique_ptr<WindowGLFW> m_window;
        std::unique_ptr<GraphicsDevice> m_graphicsDevice;
        Program* m_program = nullptr;
        
        std::map<std::string, GeoObject> mObjectsByNameMap;
        
        //! Texture resource map
        std::map<std::string, Texture2D*> mTexturesByName;
        
        Asset_CGLTF mAssetGLTF;
        
        std::unique_ptr<XE::Material> m_material;
        Texture2D* m_texture = nullptr;
        
        std::unique_ptr<FileStreamSource> m_streamSource;

        InputManager *m_inputManager = nullptr;
        ImageLoaderPNG m_imageLoaderPNG;
        
        bool m_shouldClose = false;
        float m_angle = 0.0f;
        
        Camera mCamera;
    };


    std::unique_ptr<Application> Application::create() {
        return std::make_unique<SandboxApp>();
    }
}
