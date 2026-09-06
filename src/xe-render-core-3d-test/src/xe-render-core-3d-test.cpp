
#include <glaze/gl.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <vector>

#include "xe/render/RenderBackend.h"
#include "xe/render/backend/glcore3-api.h"

#include <xe/geometry/PlaneGenerator.h>
#include <xe/math/Math.h>

namespace {
    float checkerboardValue(int x, int y, int tileSize) {
        return (((x / tileSize) + (y / tileSize)) & 1) ? 0.0f : 1.0f;
    }

    int channelCountOf(xe::PixelFormat format) {
        switch (format) {
        case xe::PixelFormat::R8G8B8:
            return 3;
        case xe::PixelFormat::R8G8B8A8:
            return 4;
        default:
            return 0;
        }
    }

    size_t byteSizeOf(xe::PixelDataType type) {
        return xe::toBytes(xe::getElementSize(static_cast<xe::TypeEncoding>(type)));
    }

    void writeNormalizedChannel(void *base, size_t pixelOffset, int channel, float value, xe::PixelDataType type) {
        auto *byteBase = static_cast<uint8_t *>(base) + pixelOffset;
        switch (type) {
        case xe::PixelDataType::UInt8:
            byteBase[channel] = static_cast<uint8_t>(value * 255.0f);
            break;
        case xe::PixelDataType::UInt16:
            reinterpret_cast<uint16_t *>(byteBase)[channel] = static_cast<uint16_t>(value * 65535.0f);
            break;
        case xe::PixelDataType::Float32:
            reinterpret_cast<float *>(byteBase)[channel] = value;
            break;
        default:
            assert(false && "writeNormalizedChannel: unsupported PixelDataType");
        }
    }

    template<typename T>
    class Span {
    public:
        struct Iterator {
            using iterator_category = std::forward_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;

            explicit Iterator(pointer ptr) : _ptr(ptr) {}

            reference operator*() const {
                return *_ptr;
            }

            pointer operator->() {
                return _ptr;
            }

            Iterator& operator++() {
                _ptr++;

                return *this;
            }

            Iterator& operator++(int) {
                Iterator it = *this;

                _ptr++;

                return it;
            }

            friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
                return _ptr == rhs._ptr;
            }

            friend bool operator==(const Iterator &lhs, const Iterator &rhs) {
                return _ptr != rhs._ptr;
            }

        private:
            pointer _ptr;
        };

        explicit Span() {}

        explicit Span(T* data, size_t size) : _data(data), _size(size) {
            // prevents issues where data and size have inconsistencies
            assert(data == nullptr && size == 0 || data != nullptr && size > 0);
        }

        T* data() {
            return _data;
        }

        size_t size() const {
            return _size;
        }

        explicit operator bool() const {
            return _data != nullptr;
        }

        Iterator begin() {
            return Iterator(_data);
        }

        Iterator end() {
            return Iterator(_data + _size);
        }

        T& operator[](const size_t i) {
            assert(i < size);

            return _data[i];
        }

        T operator[](const size_t i) const {
            assert(i < size);

            return _data[i];
        }

    private:
        T* _data = nullptr;
        size_t _size = 0;
    };


    template<typename T>
    Span<T> makeSpan(const std::vector<T> &values) {
        return Span<T>(values.data(), values.size());
    }

    template<typename T, size_t N>
    Span<T> makeSpan(const std::array<T, N> &values) {
        return Span<T>(values.data(), values.size());
    }

    //! computes Y coord elevation from a single horizontal point
    float surface(float x, float z) {
        return std::cos(x) * std::sin(z);
    }

    void computeSurfaceTriangles(Span<xe::vec3> &vertices, const float width, const float depth, int slices, int stacks) {
        const size_t size = (slices + 1) * (stacks + 1);
        assert(vertices.size() == size);

        xe::vec2 const d = xe::vec2{width, depth} / xe::vec2{(float)slices, (float)depth};
        xe::vec2 const init = -d * 0.5f;

        size_t index = 0;

        for (int i = 0; i < slices + 1; i++) {
            for (int j = 0; j < stacks + 1; j++) {
                xe::vec2 const point = init + xe::vec2{(float)i, (float)j} * d;
                float const y = surface(point.x, point.y);

                vertices[index++] = xe::vec3{point.x, y, point.y};
            }
        }
    }

    void computeSurfaceIndicesTriangles(Span<std::uint32_t> &indices, int slices, int stacks) {
        int const stride = slices + 1;
        std::size_t const count = 6 * stride * stacks;

        assert(indices.size() == count);

        std::size_t index = 0;
        for (int i = 0; i < slices; ++i) {
            for (int j = 0; j < stacks; ++j) {
                const std::uint32_t p0 = (i + 0) + (j + 0) * stride;
                const std::uint32_t p1 = (i + 1) + (j + 0) * stride;
                const std::uint32_t p2 = (i + 0) + (j + 1) * stride;
                const std::uint32_t p3 = (i + 1) + (j + 1) * stride;

                indices[index++] = p0;
                indices[index++] = p1;
                indices[index++] = p2;

                indices[index++] = p1;
                indices[index++] = p3;
                indices[index++] = p2;
            }
        }
    }

    void fillCheckerboardImage(void *data, size_t byteSize, int width, int height, xe::PixelFormat format, xe::PixelDataType dataType, int tileSize) {
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

    tl::expected<xe::TextureHandle, xe::BackendError> createCheckerBoardTexture(const xe::RenderDeviceBackendVTable &vtable, xe::RenderDeviceBackendContext *ctx, xe::ivec2 size) {
        constexpr int tileSize = 64;
        constexpr xe::PixelFormat format = xe::PixelFormat::R8G8B8A8;
        constexpr xe::PixelDataType dataType = xe::PixelDataType::UInt8;

        size_t const byteSize = static_cast<size_t>(size.x) * static_cast<size_t>(size.y) * 4u;
        std::vector<uint8_t> pixels(byteSize);
        fillCheckerboardImage(pixels.data(), byteSize, size.x, size.y, format, dataType, tileSize);

        xe::MipLevel const mip{pixels.data()};

        xe::TextureDescriptor desc{};
        desc.type = xe::TextureType::Tex2D;
        desc.format = format;
        desc.size = xe::ivec3(size.x, size.y, 1);
        desc.sourceFormat = format;
        desc.sourceDataType = dataType;
        desc.mipLevels = &mip;
        desc.mipLevelCount = 1;
        desc.generateMipmaps = true;

        return vtable.createTexture(ctx, desc);
    }

    /**
     * @brief Minimal free-look FPS camera state.
     * Yaw rotates around the world Y axis; pitch tilts around the camera's local right axis.
     * Movement is constrained to the XZ plane so forward/backward walks instead of flying.
     */
    struct FpsCamera {
        //! World-space eye position.
        xe::vec3 position = {0.0f, 1.7f, 6.0f};

        //! Yaw in radians; 0 looks toward -Z.
        float yaw = 0.0f;

        //! Pitch in radians; clamped to just shy of ±π/2 to avoid gimbal flip.
        float pitch = -0.15f;

        //! Units per second applied to WASD input.
        float moveSpeed = 4.0f;

        //! Radians of rotation per pixel of cursor delta.
        float lookSensitivity = 0.0025f;
    };

    xe::vec3 forwardOf(const FpsCamera &cam) {
        float const cp = std::cos(cam.pitch);
        return {std::sin(cam.yaw) * cp, std::sin(cam.pitch), -std::cos(cam.yaw) * cp};
    }

    xe::mat4 viewMatrixOf(const FpsCamera &cam) {
        xe::vec3 const f = forwardOf(cam);
        xe::vec3 const target = cam.position + f;
        return xe::mat4LookAtRH<float>(cam.position, target, xe::vec3{0.0f, 1.0f, 0.0f});
    }

    void updateFpsCamera(FpsCamera &cam, GLFWwindow *window, float dt, double &prevMouseX, double &prevMouseY, bool &mouseInitialized) {
        double mx = 0.0;
        double my = 0.0;
        glfwGetCursorPos(window, &mx, &my);
        if (!mouseInitialized) {
            prevMouseX = mx;
            prevMouseY = my;
            mouseInitialized = true;
        }
        float const dx = static_cast<float>(mx - prevMouseX);
        float const dy = static_cast<float>(my - prevMouseY);
        prevMouseX = mx;
        prevMouseY = my;

        cam.yaw += dx * cam.lookSensitivity;
        cam.pitch -= dy * cam.lookSensitivity;

        float const pitchLimit = 1.55334f; // ~89°
        if (cam.pitch > pitchLimit)
            cam.pitch = pitchLimit;
        if (cam.pitch < -pitchLimit)
            cam.pitch = -pitchLimit;

        xe::vec3 const forwardXZ = xe::normalize(xe::vec3{std::sin(cam.yaw), 0.0f, -std::cos(cam.yaw)});
        xe::vec3 const rightXZ = xe::vec3{std::cos(cam.yaw), 0.0f, std::sin(cam.yaw)};

        xe::vec3 move{0.0f, 0.0f, 0.0f};
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            move = move + forwardXZ;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            move = move - forwardXZ;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            move = move + rightXZ;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            move = move - rightXZ;

        float const len2 = xe::dot(move, move);
        if (len2 > 0.0f) {
            cam.position = cam.position + xe::normalize(move) * (cam.moveSpeed * dt);
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
} // namespace

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1024, 768, "xe-render-core-3d", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glazeLoadFunctions(glfwGetProcAddress);

    xe::RenderDeviceBackendVTable vtable;
    xe::initializeBackendTableGL(&vtable);

    auto ctxResult = vtable.createContext();
    if (!ctxResult) {
        std::cerr << "createContext failed: " << ctxResult.error().message << std::endl;
        return 1;
    }
    xe::RenderDeviceBackendContext *ctx = *ctxResult;

    // Shader initialization. The camera matrices travel as raw uniforms (uModel / uView /
    // uProjection / uTexTile), while the directional-light parameters are shared via a
    // std140 uniform block bound at binding point kLightBlockBinding. This mirrors the two
    // uniform paths the render-backend abstraction exposes.
    constexpr uint32_t kLightBlockBinding = 0u;

    xe::ShaderProgramDescriptor shaderDesc;
    shaderDesc.glslVertexShader = R"(
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTexTile;

out vec3 vNormal;
out vec2 vTexCoord;

void main() {
    vec4 worldPos = uModel * vec4(position, 1.0);
    gl_Position = uProjection * uView * worldPos;
    vNormal = mat3(uModel) * normal;
    vTexCoord = texCoord * uTexTile;
}
)";

    shaderDesc.glslFragmentShader = R"(
#version 330 core

layout(std140) uniform LightBlock {
    vec4 uLightDirection;   // xyz = world-space direction; w unused
    vec4 uLightAmbient;     // rgb = ambient color; a unused
};

in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uTexture;

out vec4 fragColor;

void main() {
    vec3 L = normalize(uLightDirection.xyz);
    float ndotl = max(dot(normalize(vNormal), L), 0.0);
    vec3 base = texture(uTexture, vTexCoord).rgb;
    vec3 lit = (uLightAmbient.rgb + 0.8 * ndotl) * base;
    fragColor = vec4(lit, 1.0);
}
)";

    auto shaderResult = vtable.createShaderProgram(ctx, shaderDesc);
    if (!shaderResult) {
        std::cerr << "createShaderProgram failed: " << shaderResult.error().message << std::endl;
        return 1;
    }
    xe::ShaderHandle shaderHandle = *shaderResult;

    // checkerboard texture
    auto textureResult = createCheckerBoardTexture(vtable, ctx, {512, 512});
    if (!textureResult) {
        std::cerr << "createCheckerBoardTexture failed: " << textureResult.error().message << std::endl;
        return 1;
    }
    xe::TextureHandle textureHandle = *textureResult;

    // floor mesh via libxe-geometry (allocation-free two-phase API)
    xe::PlaneOptions planeOpts;
    planeOpts.division = {16, 16};
    planeOpts.size = {40.0f, 40.0f};
    xe::MeshCounts const counts = xe::computePlaneCounts(planeOpts);

    std::vector<xe::vec3> positions(counts.vertexCount);
    std::vector<xe::vec3> normals(counts.vertexCount);
    std::vector<xe::vec2> texCoords(counts.vertexCount);
    std::vector<std::uint8_t> indexBytes(counts.indexCount * xe::indexByteSize(counts.indexType));

    xe::MeshStorage<float> storage;
    storage.positions = positions.data();
    storage.normals = normals.data();
    storage.texCoords = texCoords.data();
    storage.indices = indexBytes.data();
    xe::generatePlane<float>(planeOpts, storage);

    // upload positions / normals / texCoords as three separate vertex buffers
    xe::BufferDescriptor bufferDesc{};
    bufferDesc.type = xe::BufferType::Vertex;
    bufferDesc.usage = xe::BufferUsage::StaticDraw;

    bufferDesc.data = positions.data();
    bufferDesc.size = positions.size() * sizeof(xe::vec3);
    auto positionBufferResult = vtable.createBuffer(ctx, bufferDesc);
    if (!positionBufferResult) {
        std::cerr << "createBuffer (positions) failed: " << positionBufferResult.error().message << std::endl;
        return 1;
    }
    xe::BufferHandle positionBuffer = *positionBufferResult;

    bufferDesc.data = normals.data();
    bufferDesc.size = normals.size() * sizeof(xe::vec3);
    auto normalBufferResult = vtable.createBuffer(ctx, bufferDesc);
    if (!normalBufferResult) {
        std::cerr << "createBuffer (normals) failed: " << normalBufferResult.error().message << std::endl;
        return 1;
    }
    xe::BufferHandle normalBuffer = *normalBufferResult;

    bufferDesc.data = texCoords.data();
    bufferDesc.size = texCoords.size() * sizeof(xe::vec2);
    auto texCoordBufferResult = vtable.createBuffer(ctx, bufferDesc);
    if (!texCoordBufferResult) {
        std::cerr << "createBuffer (texCoords) failed: " << texCoordBufferResult.error().message << std::endl;
        return 1;
    }
    xe::BufferHandle texCoordBuffer = *texCoordBufferResult;

    xe::BufferDescriptor indexBufferDesc{};
    indexBufferDesc.type = xe::BufferType::Index;
    indexBufferDesc.usage = xe::BufferUsage::StaticDraw;
    indexBufferDesc.data = indexBytes.data();
    indexBufferDesc.size = indexBytes.size();
    auto indexBufferResult = vtable.createBuffer(ctx, indexBufferDesc);
    if (!indexBufferResult) {
        std::cerr << "createBuffer (indices) failed: " << indexBufferResult.error().message << std::endl;
        return 1;
    }
    xe::BufferHandle indexBuffer = *indexBufferResult;

    // vertex layout: explicit locations 0 / 1 / 2 matching the shader.
    xe::VertexLayoutDescriptor layoutDesc;
    layoutDesc.resolveMode = xe::VertexLayoutResolveMode::Explicit;
    layoutDesc.indexType = (counts.indexType == xe::IndexType::UInt16) ? xe::GeometryIndexType::uint16 : xe::GeometryIndexType::uint32;
    layoutDesc.attribs = {
        { 0, xe::VertexAttribFormat::float3, false},
        { 1, xe::VertexAttribFormat::float3, false},
        { 2, xe::VertexAttribFormat::float2, false},
    };

    auto layoutResult = vtable.createVertexLayout(ctx, layoutDesc);
    if (!layoutResult) {
        std::cerr << "createVertexLayout failed: " << layoutResult.error().message << std::endl;
        return 1;
    }
    xe::VertexLayoutHandle layout = *layoutResult;

    xe::GeometryDescriptor geometryDesc;
    geometryDesc.layoutHandle = layout;
    geometryDesc.bufferAttribs = {
        xe::GeometryBufferAttrib{positionBuffer, 0},
        xe::GeometryBufferAttrib{normalBuffer, 1},
        xe::GeometryBufferAttrib{texCoordBuffer, 2},
    };
    geometryDesc.indexBufferHandle = indexBuffer;

    auto geometryResult = vtable.createGeometry(ctx, geometryDesc);
    if (!geometryResult) {
        std::cerr << "createGeometry failed: " << geometryResult.error().message << std::endl;
        return 1;
    }
    xe::GeometryHandle geometryHandle = *geometryResult;

    // Static directional-light data, laid out to match std140 (two vec4s).
    struct LightBlockData {
        xe::vec4 direction;
        xe::vec4 ambient;
    };
    LightBlockData const lightBlock{
        xe::normalize(xe::vec4{-0.4f, 1.0f, -0.3f, 0.0f}),
        xe::vec4{0.2f, 0.2f, 0.2f, 1.0f},
    };

    xe::BufferDescriptor lightUboDesc{};
    lightUboDesc.type = xe::BufferType::Uniform;
    lightUboDesc.usage = xe::BufferUsage::StaticDraw;
    lightUboDesc.data = &lightBlock;
    lightUboDesc.size = sizeof(lightBlock);
    auto lightUboResult = vtable.createBuffer(ctx, lightUboDesc);
    if (!lightUboResult) {
        std::cerr << "createBuffer (lightUbo) failed: " << lightUboResult.error().message << std::endl;
        return 1;
    }
    xe::BufferHandle lightUbo = *lightUboResult;

    // Pipeline bakes the LightBlock → kLightBlockBinding mapping once via glUniformBlockBinding.
    xe::PipelineDescriptor pipelineDesc;
    pipelineDesc.layoutHandle = layout;
    pipelineDesc.shaderProgramHandle = shaderHandle;
    pipelineDesc.uniformBlocks.push_back({"LightBlock", kLightBlockBinding});
    auto pipelineResult = vtable.createPipeline(ctx, pipelineDesc);
    if (!pipelineResult) {
        std::cerr << "createPipeline failed: " << pipelineResult.error().message << std::endl;
        return 1;
    }
    xe::PipelineHandle const pipelineHandle = *pipelineResult;

    // Resolve every raw uniform location up front. Each returned UniformLocation is keyed to
    // shaderHandle; applyUniforms asserts in debug that the location is used against the
    // matching program, catching accidental program mixups.
    auto resolve = [&](const char *name) {
        auto r = vtable.resolveUniformLocation(ctx, shaderHandle, name);
        if (!r) {
            std::cerr << "resolveUniformLocation(" << name << ") failed: " << r.error().message << std::endl;
            std::exit(1);
        }
        return *r;
    };
    xe::UniformLocation const uModelLoc = resolve("uModel");
    xe::UniformLocation const uViewLoc = resolve("uView");
    xe::UniformLocation const uProjectionLoc = resolve("uProjection");
    xe::UniformLocation const uTextureLoc = resolve("uTexture");
    xe::UniformLocation const uTexTileLoc = resolve("uTexTile");

    auto glctxgl = static_cast<xe::RenderDeviceBackendContextGL *>(ctx);
    gl::VertexArray const vao = glctxgl->geometries[geometryHandle.index()].obj->vao.get();
    gl::Texture const textureId = glctxgl->textures[textureHandle.index()].obj->texture.get();
    gl::DrawElementsType const indexDataType = glctxgl->geometries[geometryHandle.index()].obj->indexDataType;
    GLsizei const indexCount = static_cast<GLsizei>(counts.indexCount);

    gl::enable(gl::EnableCap::eDepthTest);
    gl::depthFunc(gl::DepthFunction::eLess);

    // Floor's natural orientation is the XY plane with normal -Z; rotate so it sits on the XZ
    // plane with normal +Y. The rotation is orthonormal, so it doubles as its own normal matrix
    // in the vertex shader (mat3(uModel)).
    xe::mat4 const model = xe::mat4RotationX<float>(xe::radians(90.0f));

    FpsCamera camera;
    double prevMouseX = 0.0;
    double prevMouseY = 0.0;
    bool mouseInitialized = false;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double const now = glfwGetTime();
        float const dt = static_cast<float>(now - lastTime);
        lastTime = now;

        updateFpsCamera(camera, window, dt, prevMouseX, prevMouseY, mouseInitialized);

        int w = 0;
        int h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        if (h <= 0) {
            h = 1;
        }

        float const aspect = static_cast<float>(w) / static_cast<float>(h);
        xe::mat4 const projection = xe::mat4Perspective<float>(xe::radians(60.0f), aspect, 0.1f, 200.0f);
        xe::mat4 const view = viewMatrixOf(camera);

        gl::viewport(0, 0, w, h);
        gl::clearColor(0.08f, 0.10f, 0.14f, 1.0f);
        gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);

        gl::activeTexture(gl::TextureUnit::eTexture0);
        gl::bindTexture(gl::TextureTarget::eTexture2d, textureId);

        // Attach the static light UBO to its pipeline-declared binding point. Done per-frame
        // to exercise the abstraction; with a single pipeline the binding also stays live
        // across frames, so in real code this could hoist to initialization.
        vtable.bindUniformBuffer(ctx, kLightBlockBinding, lightUbo, 0u, 0u);

        constexpr GLint kTextureUnit = 0;
        constexpr float kTexTile = 8.0f;

        xe::UniformValueSubmission const valueUniforms[] = {
            {uTextureLoc, xe::UniformVectorType::Int1, 1u, &kTextureUnit},
            {uTexTileLoc, xe::UniformVectorType::Float1, 1u, &kTexTile},
        };

        xe::UniformMatrixSubmission const matrixUniforms[] = {
            {uModelLoc, xe::UniformMatrixShape::R4C4, 1u, false, model.data()},
            {uViewLoc, xe::UniformMatrixShape::R4C4, 1u, false, view.data()},
            {uProjectionLoc, xe::UniformMatrixShape::R4C4, 1u, false, projection.data()},
        };

        vtable
            .applyUniforms(ctx, shaderHandle, valueUniforms, sizeof(valueUniforms) / sizeof(valueUniforms[0]), matrixUniforms, sizeof(matrixUniforms) / sizeof(matrixUniforms[0]));

        gl::bindVertexArray(vao);
        gl::drawElements(gl::PrimitiveType::eTriangles, indexCount, indexDataType, nullptr);

        gl::flush();

        glfwSwapBuffers(window);
    }

    vtable.destroyPipeline(ctx, pipelineHandle);
    vtable.destroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
