
#include "DemoRenderable.h"

DemoRenderable::DemoRenderable(XE::Subset *subset, const XE::SubsetEnvelope &subsetEnvelope) : mSubset(subset), mSubsetEnvelope(subsetEnvelope) {}

void DemoRenderable::render(XE::GraphicsDevice *graphicsDevice) { graphicsDevice->draw(mSubset, &mSubsetEnvelope, 1); }
