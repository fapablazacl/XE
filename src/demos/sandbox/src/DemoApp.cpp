
#include "DemoApp.h"

namespace demo {

    int DemoApp::run(int, char **) {
        const XE::GraphicsBackend backend = XE::GraphicsBackend::GL_41;

        initialize(backend);
        setupGeometry();
        setupScene();

        mainLoop();

        return 0;
    }
    void DemoApp::initialize(const XE::GraphicsBackend backend) {
        const XE::Vector2i windowSize = {s_screenWidth, s_screenHeight};
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
        }

        assert(mGraphicsDevice);

        mInputManager = mWindow->getInputManager();

        mSimpleProgram = createSimpleProgram(backend);
    }
    XE::Program *DemoApp::createSimpleProgram(const XE::GraphicsBackend backend) {
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
    void DemoApp::setupGeometry() {
        const Mesh mesh = makeCubeMesh(0.25f, 0.25f, 0.25f);

        mColoredCubeSubset = createCubeSubset2(mGraphicsDevice.get(), mesh);
        mColoredCubeSubsetEnvelope = {mesh.primitive, 0, static_cast<int>(mesh.vertices.size())};

        const Mesh axisMesh = makeAxisMesh(1.0f, 1.0f, 1.0f);

        mAxisSubset = createSubset(mGraphicsDevice.get(), axisMesh);
        mAxisSubsetEnvelope = {axisMesh.primitive, 0, static_cast<int>(axisMesh.indices.size())};

        const Mesh floorMesh = makeGridMesh(1.0f, 20, 20);

        mFloorSubset = createSubset(mGraphicsDevice.get(), floorMesh);
        mFloorSubsetEnvelope = {floorMesh.primitive, 0, static_cast<int>(floorMesh.vertices.size())};

        const Mesh cubeMesh = makeCubeMesh(0.5f, 0.5f, 0.5f);

        mCubeSubset = createCubeSubset2(mGraphicsDevice.get(), cubeMesh);

        mCubeSubsetEnvelope = {floorMesh.primitive, 0, static_cast<int>(cubeMesh.vertices.size())};

        mMaterial.renderState.depthTest = true;
        mMaterial.renderState.cullBackFace = true;
    }
    void DemoApp::setupScene() {
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
        mEnemyNode->setTransformation(XE::mat4Translation({0.0f, 0.2501f, 0.0f}));
    }
    void DemoApp::mainLoop() {
        int lastTime = XE::Timer::getTick();

        while (mDone) {
            const int current = XE::Timer::getTick() - lastTime;
            const float seconds = static_cast<float>(current) / 1000.0f;

            lastTime = XE::Timer::getTick();

            update(seconds);
            renderSceneNode();
        }
    }
    void DemoApp::update(const float seconds) {
        mInputManager->poll();

        auto keyboardStatus = mInputManager->getKeyboardStatus();
        if (keyboardStatus.isPressed(XE::KeyCode::KeyEsc)) {
            mDone = false;
        }

        // update animations
        mAngle += XE::radians(60.0f * seconds);

        const XE::Matrix4 rotY1 = XE::mat4RotationY(mAngle);
        const XE::Matrix4 rotX1 = XE::mat4RotationX(mAngle);
        const XE::Matrix4 rotY2 = XE::mat4Rotation(mAngle, {0.0f, 1.0f, 0.0f});
        const XE::Matrix4 rotX2 = XE::mat4Rotation(mAngle, {1.0f, 0.0f, 0.0f});
        mLeftCubeNode->setTransformation(rotY1 * rotX1 * XE::mat4Translation({-1.25f, 0.5f, 0.0f}));
        mRightCubeNode->setTransformation(rotY2 * rotX2 * XE::mat4Translation({1.25f, 0.5f, 0.0f}));

        // update camera position
        const bool moveForward = keyboardStatus.isPressed(XE::KeyCode::KeyUp);
        const bool moveBackward = keyboardStatus.isPressed(XE::KeyCode::KeyDown);

        const bool turnLeft = keyboardStatus.isPressed(XE::KeyCode::KeyLeft);
        const bool turnRight = keyboardStatus.isPressed(XE::KeyCode::KeyRight);

        mCamera.update(seconds, moveForward, moveBackward, turnLeft, turnRight);
    }
    void DemoApp::renderSceneNode() {
        // const auto aspect = static_cast<float>(s_screenWidth) / static_cast<float>(s_screenHeight);

        const auto proj = XE::mat4Perspective(mCamera.fov, mCamera.aspectRatio, mCamera.znear, mCamera.zfar);
        const auto view = XE::mat4LookAtRH(mCamera.position, mCamera.lookAt, mCamera.up);
        const auto viewProj = view * proj;

        mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);

        mGraphicsDevice->setProgram(mSimpleProgram);

        const XE::UniformMatrix uView = {"uView", XE::DataType::Float32, XE::UniformMatrixShape::R4C4, 1};
        mGraphicsDevice->applyUniform(&uView, 1, reinterpret_cast<const void *>(view.data()));

        mGraphicsDevice->setMaterial(&mMaterial);
        mSceneNode.visit(mGraphicsDevice.get(), viewProj);

        mGraphicsDevice->endFrame();
    }
}
