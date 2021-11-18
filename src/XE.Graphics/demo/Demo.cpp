/**
 * XE Tech demo
 */

#include <XE/XE.h>
#include <XE/Timer.h>
#include <XE/IO.h>
#include <XE/Input.h>
#include <XE/Math.h>
#include <XE/Graphics.h>
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>
#include <XE/Graphics/ES2/GraphicsDeviceES2.h>
#include <XE/Graphics/GL/GraphicsDeviceGL.h>

#include <map>
#include <functional>

#include "Util.h"
#include "Common.h"
#include "Renderable.h"
#include "SceneNode.h"
#include "Camera.h"

const int s_screenWidth = 1200;
const int s_screenHeight = 800;

class DemoRenderable : public Renderable {
public:
    DemoRenderable() {}

    DemoRenderable(XE::Subset *subset, const XE::SubsetEnvelope &subsetEnvelope) 
        : mSubset(subset), mSubsetEnvelope(subsetEnvelope) {}

    void render(XE::GraphicsDevice *graphicsDevice) override {
        graphicsDevice->draw(mSubset, &mSubsetEnvelope, 1);
    }

private:
    XE::Subset *mSubset = nullptr;
    XE::SubsetEnvelope mSubsetEnvelope;
};


namespace demo {
    class DemoApp {
    public:
        DemoApp() {}

        int run(int argc, char **argv) {
            const XE::GraphicsBackend backend = XE::GraphicsBackend::GL_41;

            initialize(backend);
            setupGeometry();
            setupScene();

            mainLoop();

            return 0;
        }

    private:
        void initialize(const XE::GraphicsBackend backend) {
            const XE::Vector2i windowSize = { s_screenWidth, s_screenHeight };
            std::string title;

            XE::GraphicsContext::Descriptor descriptor;

            descriptor.backend = backend;

            if (descriptor.backend == XE::GraphicsBackend::GL_41) {
                title = "Graphics Demo Application(Driver: Desktop OpenGL)";
            } else {
                title = "Graphics Demo Application(Driver: Mobile OpenGL)";
            }

            mWindow = XE::WindowGLFW::create(descriptor, title, windowSize, false);
            
            if (backend == XE::GraphicsBackend::GL_41) {
                mGraphicsDevice = std::make_unique<XE::GraphicsDeviceGL>(mWindow->getContext());
            } else {
                mGraphicsDevice = std::make_unique<XE::GraphicsDeviceES2>(mWindow->getContext());
            }

            mInputManager = mWindow->getInputManager();

            mSimpleProgram = createSimpleProgram(backend);
        }


        XE::Program* createSimpleProgram(const XE::GraphicsBackend backend) {
            if (backend == XE::GraphicsBackend::GL_41) {
                const std::string simpleVS = loadTextFile("shaders/gl4/gl4-main.vert");
                const std::string simpleFS = loadTextFile("shaders/gl4/gl4-main.frag");

                XE::ProgramDescriptor desc = makeSimpleProgramDesc(simpleVS, simpleFS);
                XE::Program *program = mGraphicsDevice->createProgram(desc);

                return program;
            } else {
                const std::string simpleVS = loadTextFile("shaders/es/es-main.vert");
                const std::string simpleFS = loadTextFile("shaders/es/es-main.frag");

                XE::ProgramDescriptor desc = makeSimpleProgramDesc(simpleVS, simpleFS);
                XE::Program *program = mGraphicsDevice->createProgram(desc);

                return program;
            }

        }


        void setupGeometry() {
            const Mesh mesh = makeCubeMesh(0.25f, 0.25f, 0.25f);

            mColoredCubeSubset = createCubeSubset2(mGraphicsDevice.get(), mesh);
            mColoredCubeSubsetEnvelope = {
                mesh.primitive, 0, 
                static_cast<int>(mesh.vertices.size())
            };
        
            const Mesh axisMesh = makeAxisMesh(1.0f, 1.0f, 1.0f);
        
            mAxisSubset = createSubset(mGraphicsDevice.get(), axisMesh);
            mAxisSubsetEnvelope = {
                axisMesh.primitive, 0,
                static_cast<int>(axisMesh.indices.size())
            };
        
            const Mesh floorMesh = makeGridMesh(1.0f, 20, 20);

            mFloorSubset = createSubset(mGraphicsDevice.get(), floorMesh);
            mFloorSubsetEnvelope = { 
                floorMesh.primitive,
                0, 
                static_cast<int>(floorMesh.vertices.size())
            };

            const Mesh cubeMesh = makeCubeMesh(0.5f, 0.5f, 0.5f);

            mCubeSubset = createCubeSubset2(mGraphicsDevice.get(), cubeMesh);
            
            mCubeSubsetEnvelope = { 
                floorMesh.primitive, 0,
                static_cast<int>(cubeMesh.vertices.size())
            };

            mMaterial.renderState.depthTest = true;
            mMaterial.renderState.cullBackFace = true;
        }

        void setupScene() {
            mFloorRenderable = {mFloorSubset, mFloorSubsetEnvelope};
            mSceneNode.setRenderable(&mFloorRenderable);

            mAxisRenderable = {mAxisSubset, mAxisSubsetEnvelope};
            mColoredCubeRenderable = {mColoredCubeSubset, mColoredCubeSubsetEnvelope};

            mAxisNode = mSceneNode.createChild();
            mAxisNode->setRenderable(&mAxisRenderable);

            mLeftCubeNode = mSceneNode.createChild();
            mLeftCubeNode->setRenderable(&mColoredCubeRenderable);

            mRightCubeNode = mSceneNode.createChild();
            mRightCubeNode->setRenderable(&mColoredCubeRenderable);
            
            mEnemyRenderable = {mCubeSubset, mCubeSubsetEnvelope};
            mEnemyNode = mSceneNode.createChild();
            mEnemyNode->setRenderable(&mEnemyRenderable);
            mEnemyNode->setTransformation(XE::M4::translate({0.0f, 0.2501f, 0.0f}));
        }

        void mainLoop() {
            int lastTime = XE::Timer::getTick();

            while (mDone) {
                const int current = XE::Timer::getTick() - lastTime;
                const float seconds = static_cast<float>(current) / 1000.0f;

                lastTime = XE::Timer::getTick();

                update(seconds);
                renderSceneNode();
            }
        }

        void update(const float seconds) {
            mInputManager->poll();

            auto keyboardStatus = mInputManager->getKeyboardStatus();
            if (keyboardStatus.isPressed(XE::KeyCode::KeyEsc)) {
                mDone = false;
            }

            // update animations
            mAngle += XE::radians(60.0f * seconds);

            const auto rotY1 = XE::M4::rotateY(mAngle);
            const auto rotX1 = XE::M4::rotateX(mAngle);
            const auto rotY2 = XE::M4::rotate(mAngle, {0.0f, 1.0f, 0.0f});
            const auto rotX2 = XE::M4::rotate(mAngle, {1.0f, 0.0f, 0.0f});
            mLeftCubeNode->setTransformation(rotY1 * rotX1 * XE::M4::translate({-1.25f, 0.5f, 0.0f}));
            mRightCubeNode->setTransformation(rotY2 * rotX2 * XE::M4::translate({1.25f, 0.5f, 0.0f}));

            // update camera position
            const bool moveForward = keyboardStatus.isPressed(XE::KeyCode::KeyUp);
            const bool moveBackward = keyboardStatus.isPressed(XE::KeyCode::KeyDown);

            const bool turnLeft = keyboardStatus.isPressed(XE::KeyCode::KeyLeft);
            const bool turnRight = keyboardStatus.isPressed(XE::KeyCode::KeyRight);

            mCamera.update(seconds, moveForward, moveBackward, turnLeft, turnRight);
        }


        void renderSceneNode() {
            const auto aspect = static_cast<float>(s_screenWidth) / static_cast<float>(s_screenHeight);

            const auto proj = XE::M4::perspective(mCamera.fov, mCamera.aspectRatio, mCamera.znear, mCamera.zfar);
            const auto view = XE::M4::lookAt(mCamera.position, mCamera.lookAt, mCamera.up);
            const auto viewProj = view * proj;
            
            mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);

            mGraphicsDevice->setProgram(mSimpleProgram);
            
            const XE::UniformMatrix uView = {"uView", XE::DataType::Float32, XE::UniformMatrixShape::R4C4, 1};
            mGraphicsDevice->applyUniform(&uView, 1, reinterpret_cast<const std::byte*>(view.data()));
            
            mGraphicsDevice->setMaterial(&mMaterial);
            mSceneNode.visit(mGraphicsDevice.get(), viewProj);
            
            mGraphicsDevice->endFrame();
        }

    private:
        std::unique_ptr<XE::WindowGLFW> mWindow;
        std::unique_ptr<XE::GraphicsDevice> mGraphicsDevice;
        XE::InputManager *mInputManager = nullptr;

        bool mDone = true;
        float mAngle = 0.0f;

        XE::Program *mSimpleProgram = nullptr;
    
        XE::Subset *mAxisSubset = nullptr;
        XE::SubsetEnvelope mAxisSubsetEnvelope;
        
        XE::Subset *mColoredCubeSubset = nullptr;
        XE::SubsetEnvelope mColoredCubeSubsetEnvelope;

        XE::Subset *mFloorSubset = nullptr;
        XE::SubsetEnvelope mFloorSubsetEnvelope;
        
        XE::Subset *mCubeSubset = nullptr;
        XE::SubsetEnvelope mCubeSubsetEnvelope;

        SceneNode mSceneNode;

        SceneNode *mAxisNode = nullptr;
        SceneNode *mLeftCubeNode = nullptr;
        SceneNode *mRightCubeNode = nullptr;

        DemoRenderable mAxisRenderable;
        DemoRenderable mColoredCubeRenderable;
        DemoRenderable mFloorRenderable;

        XE::Material mMaterial;

        Camera mCamera;

        DemoRenderable mEnemyRenderable;
        SceneNode *mEnemyNode = nullptr;
    };
}


int main(int argc, char **argv) {
    // std::system("pwd");
    
    demo::DemoApp app;

    app.run(argc, argv);
}
