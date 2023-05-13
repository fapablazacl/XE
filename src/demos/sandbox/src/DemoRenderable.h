
#pragma once

#include "Renderable.h"
#include "xe/graphics/Subset.h"

class DemoRenderable : public Renderable {
public:
    DemoRenderable() {}

    DemoRenderable(XE::Subset *subset, const XE::SubsetEnvelope &subsetEnvelope);

    void render(XE::GraphicsDevice *graphicsDevice) override;

private:
    XE::Subset *mSubset = nullptr;
    XE::SubsetEnvelope mSubsetEnvelope;
};
