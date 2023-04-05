
#pragma once 

#include <memory>
#include <string>
#include <vector>
#include <xe/XE.h>
#include <xe/graphics/GL.h>
#include <xe/graphics/Graphics.h>
#include <xe/graphics/PNG.h>
#include <xe/math/Math.h>
#include <xe/io/IO.h>
#include <xe/input/Input.h>
#include <xe/platform/glfw/WindowGLFW.h>

#include "Asset_CGLTF.h"
#include "Assets.h"
#include "Scene.h"
#include "ecs/ECS.h"

namespace Sandbox {
    class Camera {
    public:
        XE::Vector3 position = {0.0f, 0.0f, 15.0f};
        XE::Vector3 lookAt = {0.0f, 0.0f, 0.0f};
        XE::Vector3 up = {0.0f, 1.0f, 0.0f};

        float fov = XE::radians(60.0f);
        float aspectRatio = 640.0f / 480.0f;
        float znear = 0.1f;
        float zfar = 1000.0f;

        void update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight);
        
    private:
        const float turnSpeedPerSecond = 1.25f;
        const float moveSpeedPerSecond = 0.025f;
    };

    struct GeoObject {
        std::vector<std::pair<XE::Subset *, XE::SubsetEnvelope>> subsets;
    };


    class RenderingSystem {
    public:
        explicit RenderingSystem(int argc, char **argv);

        void Initialize();

        void Update(const float seconds);

        void Render();

        bool ShouldClose() const;

    private:
        void renderScene();

        void renderObjects();

        void renderMatrices(const XE::Matrix4 &);

        XE::Texture2D *createColorTexture(const int width, const int height, const XE::Vector4 &color);

        XE::Texture2D *createFileTexture(const std::string &filePath);

        SceneDescription createSceneDescription(int argc, char **argv);

        void initializeShaders();

        std::string loadTextFile(const std::string &path) const;

        void initializeGeometry();

        std::map<std::string, GeoObject> loadGeoObject(const std::string &sceneFilePath);

        std::pair<XE::Subset *, XE::SubsetEnvelope> createSubset(const MeshPrimitive &meshPrimitive);

    private:
        SceneDescription sceneDescription;

        std::unique_ptr<XE::WindowGLFW> m_window;
        std::unique_ptr<XE::GraphicsDevice> m_graphicsDevice;
        XE::Program *m_program = nullptr;

        std::map<std::string, GeoObject> mObjectsByNameMap;

        //! Texture resource map
        std::map<std::string, XE::Texture2D *> mTexturesByName;

        Asset_CGLTF mAssetGLTF;

        std::unique_ptr<XE::Material> m_material;
        XE::Texture2D *m_texture = nullptr;

        std::unique_ptr<XE::FileStreamSource> m_streamSource;

        XE::InputManager *m_inputManager = nullptr;
        XE::ImageLoaderPNG m_imageLoaderPNG;

        bool m_shouldClose = false;
        float m_angle = 0.0f;

        Camera mCamera;
    };
} // namespace Sandbox
