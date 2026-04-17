
#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

#include "xe/render/RenderBackend.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "glaze - spinning triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glazeLoadFunctions(glfwGetProcAddress);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

		glfwSwapBuffers(window);
	}

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
