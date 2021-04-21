

#include "SandboxApp.hpp"
#include "Assets.hpp"

#include <XE/XE.hpp>
#include <XE/Math.hpp>
#include <XE/Input.hpp>
#include <XE/IO.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL.hpp>
#include <XE/Graphics/GL/GLFW/WindowGLFW.hpp>
#include <XE/Graphics/PNG.hpp>

#include <string>
#include <iostream>
#include <fstream>


namespace XE {
    struct GeoObject {
        std::vector<std::pair<
            std::unique_ptr<Subset>,
            SubsetEnvelope>> subsets;
    };


    class SandboxApp : public Application {
    public:
        explicit SandboxApp(const std::vector<std::string> &args) {}
        
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
            // const Vector2i windowSize = m_window->getSize();
            // m_graphicsDevice->setViewport({{0, 0}, windowSize});

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

            // camera turning
            const auto cameraDirection = normalize(m_cameraLookAt - m_cameraPosition);
            float turnSpeed = 0.0f;
            if (turnLeft) {
                turnSpeed = -radians(1.25f * seconds);
            } else if (turnRight) {
                turnSpeed = radians(1.25f * seconds);
            }

            const auto cdt = Matrix4f::createRotation(turnSpeed, m_cameraUp) * Vector4f(cameraDirection, 0.0f);
            
            m_cameraLookAt = m_cameraPosition + Vector3f{cdt.X, cdt.Y, cdt.Z} * norm(cameraDirection);

            // camera movement
            const auto cameraSpeed = 0.025f * seconds;
            const auto cameraDisplacement = cameraDirection * cameraSpeed * seconds;
            const auto cameraSide = normalize(cross(cameraDirection, m_cameraUp));
            
            if (moveForward) {
                m_cameraPosition += cameraDisplacement;
                m_cameraLookAt += cameraDisplacement;
            } else if (moveBackward) {
                m_cameraPosition -= cameraDisplacement;
                m_cameraLookAt -= cameraDisplacement;
            }
        }
        

        void renderMatrices(const XE::Matrix4f &model = XE::Matrix4f::createIdentity()) {
            const UniformMatrix matrixLayout = { "m_mvp", DataType::Float32, 4, 4, 1 };

            const Matrix4f modelViewProj = transpose (
                Matrix4f::createPerspective(m_cameraFov, m_cameraAspect, m_cameraZNear, m_cameraZFar) * 
                Matrix4f::createLookAt(m_cameraPosition, m_cameraLookAt, m_cameraUp) * 
                Matrix4f::createRotationY(radians(m_angle)) 
            );

            m_graphicsDevice->applyUniform(&matrixLayout, 1, (const std::byte*)&modelViewProj);
        }

        
        std::unique_ptr<Texture2D> createColorTexture(const int width, const int height, const Vector4f &color) {
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
        

        std::unique_ptr<Texture2D> createFileTexture(const std::string &filePath) {
            assert(m_streamSource);
            
            std::cout << "SandboxApp::CreateFileTexture: Loading texture from file " << filePath << " ..." << std::endl;
            auto stream = m_streamSource->open(filePath);

            auto imagePtr = m_imageLoaderPNG.load(stream.get());

            return m_graphicsDevice->createTexture2D (
                PixelFormat::R8G8B8A8, 
                imagePtr->getSize(), 
                imagePtr->getFormat(),
                DataType::UInt8,
                imagePtr->getPointer()
            );
        }

        
        void Render() override {
            m_graphicsDevice->beginFrame(ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);
            m_graphicsDevice->setProgram(m_program.get());

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
                       subset_N_envelope.first.get(),
                       &subset_N_envelope.second, 1);
                }
            });
        }
        
        
        void renderObjects() {
            for (const auto &pair : mObjectsByNameMap) {
                for (const auto &subset_N_envelope : pair.second.subsets) {
                    m_graphicsDevice->draw(
                       subset_N_envelope.first.get(),
                       &subset_N_envelope.second, 1);
                }
            }
        }
        
        void initializeShaders() {
            // setup program shader
            const ProgramDescriptor programDescriptor = {
                {
                    {ShaderType::Vertex, this->getShaderSource("media/shaders/simple/vertex.glsl")},
                    {ShaderType::Fragment, this->getShaderSource("media/shaders/simple/fragment.glsl")}
                }
            };

            m_program = m_graphicsDevice->createProgram(programDescriptor);
        }

        
        std::string getShaderSource(const std::string &path) const {
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

            m_material = std::make_unique<Material>();
            m_material->layers[0].texture = m_texture.get();
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
        
        
        std::pair<std::unique_ptr<Subset>, SubsetEnvelope> createSubset(const MeshPrimitive &meshPrimitive) {
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

            std::vector<std::unique_ptr<Buffer>> buffers;
            buffers.push_back(std::move(coordBuffer));
            buffers.push_back(std::move(colorBuffer));
            buffers.push_back(std::move(normalBuffer));
            buffers.push_back(std::move(texCoordBuffer));

            std::map<std::string, int> bufferMapping = {
                {"vertCoord", 0}, {"vertColor", 1}, {"vertNormal", 2}, {"vertTexCoord", 3}
            };

            return {
                m_graphicsDevice->createSubset(subsetDescriptor, std::move(buffers), bufferMapping, std::move(indexBuffer)),
                meshPrimitive.getEnvelope()
            };
        }
        
    private:
        std::unique_ptr<WindowGLFW> m_window;
        std::unique_ptr<GraphicsDevice> m_graphicsDevice;
        std::unique_ptr<Program> m_program;
        
        std::map<std::string, GeoObject> mObjectsByNameMap;
        
        Asset_CGLTF mAssetGLTF;
        
        std::unique_ptr<Material> m_material;
        std::unique_ptr<Texture2D> m_texture;
        
        std::unique_ptr<FileStreamSource> m_streamSource;

        InputManager *m_inputManager = nullptr;
        ImageLoaderPNG m_imageLoaderPNG;
        
        bool m_shouldClose = false;

        float m_angle = 0.0f;

        Vector3f m_cameraPosition = {0.0f, 0.0f, 15.0f};
        Vector3f m_cameraLookAt = {0.0f, 0.0f, 0.0f};
        Vector3f m_cameraUp = {0.0f, 1.0f, 0.0f};

        float m_cameraFov = radians(60.0f);
        float m_cameraAspect = 640.0f / 480.0f;
        float m_cameraZNear = 0.1f;
        float m_cameraZFar = 1000.0f;
    };


    std::unique_ptr<Application> Application::create(const std::vector<std::string> &args) {
        return std::make_unique<SandboxApp>(args);
    }
}
