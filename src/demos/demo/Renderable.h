
#pragma once 

#include <xe/graphics.h>

class Renderable {
public:
    virtual ~Renderable() {}

    virtual void render(XE::GraphicsDevice *graphicsDevice) = 0;
};
