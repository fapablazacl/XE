
#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cassert>

#include "xe/render/RenderBackend.h"
#include "xe/render/backend/glcore3-api.h"

// TODO: The quick initial image generation infra needs to be refactored later
static float checkerboardValue(int x, int y, int tileSize) {
    return (((x / tileSize) + (y / tileSize)) & 1) ? 0.0f : 1.0f;
}

static int channelCountOf(xe::PixelFormat format) {
    switch (format) {
    case xe::PixelFormat::R8G8B8:   return 3;
    case xe::PixelFormat::R8G8B8A8: return 4;
    default: return 0;
    }
}

static size_t byteSizeOf(xe::DataType type) {
    switch (type) {
    case xe::DataType::Int8:   case xe::DataType::UInt8:   return 1;
    case xe::DataType::Int16:  case xe::DataType::UInt16:  case xe::DataType::Float16: return 2;
    case xe::DataType::Int32:  case xe::DataType::UInt32:  case xe::DataType::Float32: return 4;
    default: return 0;
    }
}

static void writeNormalizedChannel(void* base, size_t pixelOffset, int channel, float value, xe::DataType type) {
    auto* byteBase = static_cast<uint8_t*>(base) + pixelOffset;
    switch (type) {
    case xe::DataType::UInt8:
        byteBase[channel] = static_cast<uint8_t>(value * 255.0f);
        break;
    case xe::DataType::UInt16:
        reinterpret_cast<uint16_t*>(byteBase)[channel] = static_cast<uint16_t>(value * 65535.0f);
        break;
    case xe::DataType::Float32:
        reinterpret_cast<float*>(byteBase)[channel] = value;
        break;
    default:
        assert(false && "writeNormalizedChannel: unsupported DataType");
    }
}

static void fillCheckerboardImage(void* data, size_t byteSize, int width, int height,
                                  xe::PixelFormat format, xe::DataType dataType, int tileSize) {
    int const channels = channelCountOf(format);
    size_t const channelBytes = byteSizeOf(dataType);
    size_t const pixelStride = static_cast<size_t>(channels) * channelBytes;

    assert(data != nullptr);
    assert(channels > 0 && channelBytes > 0);
    assert(byteSize >= static_cast<size_t>(width) * height * pixelStride);
    (void)byteSize;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float const v = checkerboardValue(x, y, tileSize);
            size_t const pixelOffset = (static_cast<size_t>(y) * width + x) * pixelStride;
            for (int c = 0; c < channels; ++c) {
                float const out = (c == 3) ? 1.0f : v;
                writeNormalizedChannel(data, pixelOffset, c, out, dataType);
            }
        }
    }
}

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
