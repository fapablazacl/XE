
#include "Window.h"
#include "GltfRenderer.h"

int main(int argc,char *argv[]) {
    using xe::gltf_view::GltfRenderer;

    Window window;
    window.initialize();

    GltfRenderer renderer;

    while (window.pollInput()) {
        window.update();
        renderer.render();
        window.render();
    }

    return 0;
}
