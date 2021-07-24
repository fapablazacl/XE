/**
 * XE Tech demo
 */

#include <XE/XE.h>
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
    virtual ~Renderable() = 0 {}

    virtual void render() = 0;

    virtual void render(XE::GraphicsDevice *graphicsDevice) {

    }
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
    
    void visit(const XE::M4 &parentTransformation) {
        const XE::M4 current = parentTransformation * mTransformation;

        if (mRenderable) {
            mRenderable->render();
        }

        for (auto &child : mChildren) {
            child->visit(current);
        }
    }

private:
    SceneNode *mParent = nullptr;
    Renderable* mRenderable = nullptr;
    XE::M4 mTransformation = XE::M4::identity();
    std::vector<std::unique_ptr<SceneNode>> mChildren;
};


class DemoRenderable : public Renderable {
public:
    DemoRenderable() {}

    DemoRenderable(XE::GraphicsDevice *graphicsDevice, XE::Subset *subset, const XE::SubsetEnvelope &subsetEnvelope) 
        : mGraphicsDevice(graphicsDevice), mSubset(subset), mSubsetEnvelope(subsetEnvelope) {}

    void render() override {
        mGraphicsDevice->draw(mSubset, &mSubsetEnvelope, 1);
    }

private:
    XE::GraphicsDevice *mGraphicsDevice = nullptr;
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
            const Mesh mesh = makeCubeMesh(0.25f, 0.25f, 0.25f);

            mCubeSubset = createCubeSubset2(mGraphicsDevice.get(), mesh);
            mCubeSubsetEnvelope = {
                mesh.primitive,
                0, 
                static_cast<int>(mesh.vertices.size())
            };
        
            const Mesh axisMesh = makeAxisMesh(1.0f, 1.0f, 1.0f);
        
            mAxisSubset = createSubset(mGraphicsDevice.get(), axisMesh);
            mAxisSubsetEnvelope = {
                axisMesh.primitive,
                0,
                static_cast<int>(axisMesh.indices.size())
            };
        
            mMaterial.renderState.depthTest = true;
            mMaterial.renderState.cullBackFace = true;
        }

        void setupScene() {
            mAxisRenderable = {mGraphicsDevice.get(),  mAxisSubset, mAxisSubsetEnvelope};
            mCubeRenderable = {mGraphicsDevice.get(),  mCubeSubset, mCubeSubsetEnvelope};

            mAxisNode = mSceneNode.createChild();
            mAxisNode->setRenderable(&mAxisRenderable);

            mLeftCubeNode = mSceneNode.createChild();
            mLeftCubeNode->setRenderable(&mCubeRenderable);

            mRightCubeNode = mSceneNode.createChild();
            mRightCubeNode->setRenderable(&mCubeRenderable);
        }

        void mainLoop() {
            while (mDone) {
                update();
                // renderFrame();
                renderSceneNode();
            }
        }

        void update() {
            mInputManager->poll();

            auto keyboardStatus = mInputManager->getKeyboardStatus();
            if (keyboardStatus.isPressed(XE::KeyCode::KeyEsc)) {
                mDone = false;
            }

            mAngle += XE::radians(0.25f);

            const auto rotY1 = XE::M4::rotateY(mAngle);
            const auto rotX1 = XE::M4::rotateX(mAngle);
            const auto rotY2 = XE::M4::rotate(mAngle, {0.0f, 1.0f, 0.0f});
            const auto rotX2 = XE::M4::rotate(mAngle, {1.0f, 0.0f, 0.0f});
            mLeftCubeNode->setTransformation(rotY2 * rotX2 * XE::M4::translate({0.25f, 0.0f, 0.0f}));
            mRightCubeNode->setTransformation(rotY2 * rotX2 * XE::M4::translate({0.25f, 0.0f, 0.0f}));
        }

        void renderFrame() {
            const XE::UniformMatrix uProjModelView = {
                "uProjViewModel",
                XE::DataType::Float32,
                4, 4, 1
            };

            const auto aspect = static_cast<float>(s_screenWidth) / static_cast<float>(s_screenHeight);

            const auto proj = XE::M4::perspective(XE::radians(60.0f), aspect, 0.01f, 100.0f);
            const auto view = XE::M4::lookAt({0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
            const auto model = XE::M4::translate({0.0f, 0.0f, 5.0f}) * XE::M4::rotateX(mAngle) * XE::M4::rotateY(mAngle) * XE::M4::rotateZ(mAngle);
            // const auto projViewModel = model/* * view * proj*/;

            const auto rotY1 = XE::M4::rotateY(mAngle);
            const auto rotX1 = XE::M4::rotateX(mAngle);
        
            const auto rotY2 = XE::M4::rotate(mAngle, {0.0f, 1.0f, 0.0f});
            const auto rotX2 = XE::M4::rotate(mAngle, {1.0f, 0.0f, 0.0f});
        
            mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);

            mGraphicsDevice->setProgram(mSimpleProgram);
            
            {
                // left cube
                const auto projViewModel1 = rotY1 * rotX1 * XE::M4::translate({-0.25f, 0.0f, 0.0f});
                mGraphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(projViewModel1.data()));
                mGraphicsDevice->setMaterial(&mMaterial);
                mGraphicsDevice->draw(mCubeSubset, &mCubeSubsetEnvelope, 1);
            }
            
            {
                // right cube
                const auto projViewModel2 = rotY2 * rotX2 * XE::M4::translate({0.25f, 0.0f, 0.0f});
                mGraphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(projViewModel2.data()));
                mGraphicsDevice->setMaterial(&mMaterial);
                mGraphicsDevice->draw(mCubeSubset, &mCubeSubsetEnvelope, 1);
            }
        
            {
                // axis
                const auto projViewModel3 = XE::M4::identity();
                mGraphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(projViewModel3.data()));
                mGraphicsDevice->setMaterial(&mMaterial);
                mGraphicsDevice->draw(mAxisSubset, &mAxisSubsetEnvelope, 1);
            }
        
            mGraphicsDevice->endFrame();
        }

        void renderSceneNode() {
            const XE::UniformMatrix uProjModelView = {"uProjViewModel", XE::DataType::Float32, 4, 4, 1};

            mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);

            mGraphicsDevice->setProgram(mSimpleProgram);

            XE::M4 projViewModel = XE::M4::identity();

            mSceneNode.visit([this, &projViewModel, uProjModelView](SceneNode *node) {
                projViewModel *= node->getTransformation();
                this->mGraphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(projViewModel.data()));

                if (node->getRenderable()) {
                    node->getRenderable()->render();
                }
            });

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
        
        XE::Subset *mCubeSubset = nullptr;
        XE::SubsetEnvelope mCubeSubsetEnvelope;

        SceneNode mSceneNode;

        SceneNode *mAxisNode = nullptr;
        SceneNode *mLeftCubeNode = nullptr;
        SceneNode *mRightCubeNode = nullptr;

        DemoRenderable mAxisRenderable;
        DemoRenderable mCubeRenderable;

        XE::Material mMaterial;
    };
}


int main(int argc, char **argv) {
    demo::DemoApp app;

    app.run(argc, argv);
}
