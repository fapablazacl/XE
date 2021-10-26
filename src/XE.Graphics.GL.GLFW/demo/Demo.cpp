/**
 * XE Tech demo
 */

#include <XE/XE.h>
#include <XE/Timer.h>
#include <XE/IO.h>
#include <XE/Input.h>
#include <XE/Math.h>
#include <XE/Graphics.h>
#include <XE/Graphics/GL.h>
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>

#include <map>
#include <functional>

#include "Util.h"
#include "Common.h"

const int s_screenWidth = 1200;
const int s_screenHeight = 800;

class Renderable {
public:
    virtual ~Renderable() {}

    virtual void render(XE::GraphicsDevice *graphicsDevice) = 0;
};

class SceneNode;

using SceneNodeVisitor = std::function<void (SceneNode *)>;

class SceneNode {
public:
    SceneNode(SceneNode *parent = nullptr) : mParent(parent) {}

    ~SceneNode() {}

    SceneNode* getParent() const {
        return mParent;
    }

    XE::M4 getTransformation() const {
        return mTransformation;
    }

    void setTransformation(const XE::M4 &transformation) {
        mTransformation = transformation;
    }

    void setRenderable(Renderable *renderable) {
        mRenderable = renderable;
    }

    Renderable* getRenderable() const {
        return mRenderable;
    }

    SceneNode* createChild() {
        auto child = new SceneNode{this};

        mChildren.emplace_back(child);

        return child;
    }

    void visit(SceneNodeVisitor visitor) {
        visitor(this);

        for (auto &child : mChildren) {
            child->visit(visitor);
        }
    }
    
    void visit(XE::GraphicsDevice *graphicsDevice, const XE::M4 &parentTransformation) {
        assert(graphicsDevice);

        const XE::M4 current = mTransformation * parentTransformation;

        if (mRenderable) {
            const XE::UniformMatrix uProjModelView = {"uProjViewModel", XE::DataType::Float32, 4, 4, 1};
            graphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(current.data()));

            mRenderable->render(graphicsDevice);
        }

        for (auto &child : mChildren) {
            child->visit(graphicsDevice, current);
        }
    }

private:
    SceneNode *mParent = nullptr;
    Renderable* mRenderable = nullptr;
    XE::M4 mTransformation = XE::M4::identity();
    std::vector<std::unique_ptr<SceneNode>> mChildren;
};

struct Camera {
    XE::Vector3f position = {0.0f, 0.5f, 2.5f};
    XE::Vector3f lookAt = {0.0f, 0.0f, 0.0f};
    XE::Vector3f up = {0.0f, 1.0f, 0.0f};

    float fov = XE::radians(60.0f);
    float aspectRatio = static_cast<float>(s_screenWidth) / static_cast<float>(s_screenHeight);
    float znear = 0.01f;
    float zfar = 1000.0f;
        
    void update(const float seconds, const bool moveForward, const bool moveBackward, const bool turnLeft, const bool turnRight) {
        const float speed = 2.0f;

        XE::Vector3f forward = {0.0f, 0.0f, 0.0f};

        if (moveForward) {
            forward = {0.0f, 0.0f, -1.0f};
        } else if (moveBackward) {
            forward = {0.0f, 0.0f, 1.0f};
        }

        XE::Vector3f side = {0.0f, 0.0f, 0.0f};
        if (turnLeft) {
            side = {-1.0f, 0.0f, 0.0f};
        } else if (turnRight) {
            side = {1.0f, 0.0f, 0.0f};
        }

        const XE::Vector3f displacement = seconds * speed * (forward + side);

        position += displacement;
        lookAt += displacement;
    }
};


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
            initialize();
            setupGeometry();
            setupScene();

            mainLoop();

            return 0;
        }

    private:
        void initialize() {
            mWindow = XE::WindowGLFW::create(XE::ContextDescriptorGL::defaultGL4(), "XE.Demo example application", {s_screenWidth, s_screenHeight}, false);
            mGraphicsDevice = std::make_unique<XE::GraphicsDeviceGL>(mWindow->getContext());
            mInputManager = mWindow->getInputManager();

            mSimpleProgram = createSimpleProgram();
        }

        XE::Program* createSimpleProgram() {
            const std::string simpleVS = loadTextFile("shaders/gl4/main.vert");
            const std::string simpleFS = loadTextFile("shaders/gl4/main.frag");

            XE::ProgramDescriptor desc = makeSimpleProgramDesc(simpleVS, simpleFS);
            XE::Program *program = mGraphicsDevice->createProgram(desc);

            return program;
        }

        void setupGeometry() {
            const Mesh mesh = makeColoredCubeMesh(0.25f, 0.25f, 0.25f);

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
                floorMesh.primitive, 0, 
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
            // TODO: Commented
            mRightCubeNode->setRenderable(&mColoredCubeRenderable);

            mEnemyRenderable = {mCubeSubset, mCubeSubsetEnvelope};
            mEnemyNode = mSceneNode.createChild();
            // TODO: Commented
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
            
            mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.0f, 0.0f, 0.0f, 1.0f}, 1.0f, 0);

            mGraphicsDevice->setProgram(mSimpleProgram);
            mGraphicsDevice->setMaterial(&mMaterial);
            mSceneNode.visit(mGraphicsDevice.get(), viewProj);
            
            mGraphicsDevice->endFrame();
        }

    private:
        std::unique_ptr<XE::WindowGLFW> mWindow;
        std::unique_ptr<XE::GraphicsDeviceGL> mGraphicsDevice;
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
    demo::DemoApp app;

    app.run(argc, argv);
}
