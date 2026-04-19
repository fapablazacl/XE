
#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <vector>

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

xe::TextureHandle createCheckerBoardTexture(const xe::RenderDeviceBackendVTable &vtable, xe::RenderDeviceBackendContext *ctx, xe::ivec2 size) {
    constexpr int tileSize = 64;
    constexpr xe::PixelFormat format = xe::PixelFormat::R8G8B8A8;
    constexpr xe::DataType dataType = xe::DataType::UInt8;

    size_t const byteSize = static_cast<size_t>(size.x) * static_cast<size_t>(size.y) * 4u;
    std::vector<uint8_t> pixels(byteSize);
    fillCheckerboardImage(pixels.data(), byteSize, size.x, size.y, format, dataType, tileSize);

    xe::MipLevel const mip{ pixels.data() };

    xe::TextureDescriptor desc{};
    desc.type = xe::TextureType::Tex2D;
    desc.format = format;
    desc.size = xe::ivec3(size, 1);
    desc.sourceFormat = format;
    desc.sourceDataType = dataType;
    desc.mipLevels = &mip;
    desc.mipLevelCount = 1;
    desc.generateMipmaps = true;

    return vtable.createTexture(ctx, desc);
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "render backend test", nullptr, nullptr);
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
	auto glctxgl = static_cast<xe::RenderDeviceBackendContextGL*>(ctx);

    // shader initialization
    xe::ShaderProgramDescriptor shaderDesc;
    shaderDesc.glslVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 position; 
layout(location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(position, 1.0);
    fragTexCoord = texCoord;
}
)";

    shaderDesc.glslFragmentShader = R"(
#version 330 core

in vec2 fragTexCoord;

uniform sampler2D uTexture;

out vec4 fragColor;
void main() {
    vec4 color = texture(uTexture, fragTexCoord);
    fragColor = color;
}
)";

    xe::ShaderHandle shaderHandle = vtable.createShaderProgram(ctx, shaderDesc);
	/*if (!shaderHandle.isValid()) {
		std::cerr << "Shader program initialization failed." << std::endl;
		return 1;
	}*/
    gl::Program programId = glctxgl->shaderPrograms[shaderHandle.index()].obj.get();

    // texture generation
    xe::TextureHandle textureHandle = createCheckerBoardTexture(vtable, ctx, {512, 512});
    gl::Texture textureId = glctxgl->textures[textureHandle.index()].obj.get();

    // vertex buffer initialization
    xe::vec3 const verts[] = {{-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {-0.5f, -0.5, 0.0f}, {0.5, -0.5, 0.0}};
    xe::vec2 const texCoords[] = {{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};

    xe::BufferDescriptor bufferDesc{};
    bufferDesc.type = xe::BufferType::Vertex;
    bufferDesc.usage = xe::BufferUsage::DynamicDraw;
    bufferDesc.data = verts[0].data();
    bufferDesc.size = sizeof(verts);
    xe::BufferHandle vertexBuffer = vtable.createBuffer(ctx, bufferDesc);

    bufferDesc.data = texCoords[0].data();
    bufferDesc.size = sizeof(texCoords);
    xe::BufferHandle texCoordBuffer = vtable.createBuffer(ctx, bufferDesc);

    // get native GL buffer id to manually create a VAO for rendering testing purposes
    const gl::BufferId vertexBufferId = glctxgl->buffers[vertexBuffer.index()].obj.get();
    const gl::BufferId texCoordBufferId = glctxgl->buffers[texCoordBuffer.index()].obj.get();

    gl::VertexArray vao = gl::createVertexArrays();

    gl::bindVertexArray(vao);
    // vertex buffer
    gl::bindBuffer(gl::BufferTarget::eArrayBuffer, vertexBufferId);
    gl::enableVertexArrayAttrib(vao, 0);
    gl::vertexAttribPointer(gl::AttribLocation{0}, 3, gl::VertexAttribPointerType::eFloat, GL_FALSE, 0, nullptr);

    // texcoord buffer
	gl::bindBuffer(gl::BufferTarget::eArrayBuffer, texCoordBufferId);
	gl::enableVertexArrayAttrib(vao, 1);
	gl::vertexAttribPointer(gl::AttribLocation{ 1 }, 2, gl::VertexAttribPointerType::eFloat, GL_FALSE, 0, nullptr);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

        gl::clear(gl::ClearBufferMask::eColorBufferBit);

        gl::viewport(0, 0, w, h);

        gl::useProgram(programId);

        gl::UniformLocation textureLoc = gl::getUniformLocation(programId, "uTexture");

        gl::activeTexture(gl::TextureUnit::eTexture0);
        gl::bindTexture(gl::TextureTarget::eTexture2d, textureId);
        gl::uniform1i(textureLoc, 0);

        gl::bindVertexArray(vao);
        gl::drawArrays(gl::PrimitiveType::eTriangleStrip, 0, 4);
        gl::flush();

		glfwSwapBuffers(window);
	}

    vtable.destroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
