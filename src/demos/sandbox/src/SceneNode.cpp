
#include "SceneNode.h"

SceneNode *SceneNode::createChild() {
    auto child = new SceneNode{this};

    mChildren.emplace_back(child);

    return child;
}

void SceneNode::visit(SceneNodeVisitor visitor) {
    visitor(this);

    for (auto &child : mChildren) {
        child->visit(visitor);
    }
}

void SceneNode::visit(XE::GraphicsDevice *graphicsDevice, const XE::M4 &parentTransformation) {
    assert(graphicsDevice);

    const XE::M4 current = mTransformation * parentTransformation;

    if (mRenderable) {
        const XE::UniformMatrix uProjModelView = {"uProjViewModel", XE::DataType::Float32, XE::UniformMatrixShape::R4C4, 1};
        graphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const void *>(current.data()));

        const XE::UniformMatrix uModel = {"uModel", XE::DataType::Float32, XE::UniformMatrixShape::R4C4, 1};
        graphicsDevice->applyUniform(&uModel, 1, reinterpret_cast<const void *>(mTransformation.data()));

        mRenderable->render(graphicsDevice);
    }

    for (auto &child : mChildren) {
        child->visit(graphicsDevice, current);
    }
}
