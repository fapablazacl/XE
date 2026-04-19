
#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

#include "xe/render/RenderBackend.h"
#include "xe/render/backend/glcore3-api.h"

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

    xe::RenderDeviceBackendVTable vtable;
    xe::initializeBackendTableGL(&vtable);

    xe::RenderDeviceBackendContext* ctx = vtable.createContext();

    // shader initialization
    xe::ShaderProgramDescriptor shaderDesc;
    shaderDesc.glslVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 position; 
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

    shaderDesc.glslFragmentShader = R"(
#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

    xe::Handle shaderHandle = vtable.createShaderProgram(ctx, shaderDesc);
    if (!shaderHandle.type()) {
        // TODO: Implement an Handle API for checking for invalid/empty handles. This one doesn't work
        std::cerr << "Shader program initialization failed." << std::endl;
        return 1;
    }

    // vertex buffer initialization
    xe::vec3 const verts[] = {{-5.0f, 0.5f, 0.0f}, {5.0f, 0.5f, 0.0f}, {-0.5f, -0.5, 0.0f}, {0.5, -0.5, 0.0}};
    xe::vec2 const texCoords[] = {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};

    xe::BufferDescriptor bufferDesc{};
    bufferDesc.type = xe::BufferType::Vertex;
    bufferDesc.usage = xe::BufferUsage::DynamicDraw;
    bufferDesc.data = verts[0].data();
    bufferDesc.size = sizeof(xe::vec3) * 3;
    xe::Handle vertexBuffer = vtable.createBuffer(ctx, bufferDesc);

    bufferDesc.data = texCoords[0].data();
    bufferDesc.size = sizeof(xe::vec2) * 3;
    xe::Handle texCoordBuffer = vtable.createBuffer(ctx, bufferDesc);

    // get native GL buffer id to manually create a VAO for rendering testing purposes


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

		glfwSwapBuffers(window);
	}

    vtable.destroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
