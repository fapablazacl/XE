
#include "GltfRenderer.h"
#include "Window.h"

int main(int argc, char *argv[]) {
    using xe::gltf_view::GltfRenderer;

    Window window;
    window.initialize();

    GltfRenderer renderer;

    window.setData(renderer.getData());

    while (window.pollInput()) {
        window.update();
        renderer.beginFrame();

        window.prepareUI();
        // render the geometries
        renderer.render();

        // render the UI
        window.drawUI();

        // present the frame
        renderer.endFrame();
        window.swapBuffers();
    }

    return 0;
}
