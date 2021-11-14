
#pragma once 

#include <XE/Graphics.h>

class Renderable {
public:
    virtual ~Renderable() {}

    virtual void render(XE::GraphicsDevice *graphicsDevice) = 0;
};
