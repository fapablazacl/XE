
#pragma once 

#include <XE/Math.h>
#include <functional>

#include "Renderable.h"

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

    SceneNode* createChild();

    void visit(SceneNodeVisitor visitor);
    
    void visit(XE::GraphicsDevice* graphicsDevice, const XE::M4& parentTransformation);

private:
    SceneNode *mParent = nullptr;
    Renderable* mRenderable = nullptr;
    XE::M4 mTransformation = XE::M4::identity();
    std::vector<std::unique_ptr<SceneNode>> mChildren;
};
