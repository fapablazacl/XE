/**
* XE Tech demo
*/

#pragma once

#include <xe/Timer.h>
#include <xe/XE.h>
#include <xe/graphics/Graphics.h>
#include <xe/platform/glfw/WindowGLFW.h>
#include <xe/graphics/GL.h>
#include <xe/input/Input.h>
#include <xe/math/Math.h>

#include <functional>
#include <map>

#include "Camera.h"
#include "Common.h"
#include "DemoRenderable.h"
#include "Renderable.h"
#include "SceneNode.h"
#include "Util.h"

const int s_screenWidth = 1200;
const int s_screenHeight = 800;

namespace demo {
   class DemoApp {
   public:
       DemoApp() {}

       int run(int , char **);

   private:
       void initialize(const XE::GraphicsBackend backend);

       XE::Program *createSimpleProgram(const XE::GraphicsBackend backend);

       void setupGeometry();

       void setupScene();

       void mainLoop();

       void update(const float seconds);

       void renderSceneNode();

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
} // namespace demo
