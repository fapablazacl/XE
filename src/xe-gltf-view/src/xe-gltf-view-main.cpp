
#include "Window.h"
#include "GltfRenderer.h"

int main(int argc,char *argv[]) {
    Window window;

    window.initialize();
    window.loop();

    return 0;
}
