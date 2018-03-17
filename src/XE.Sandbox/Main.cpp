
#include <array>
#include <string>
#include <iostream>

#include <XE.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL/GraphicsDeviceGL.hpp>

namespace XE::Graphics {
    /**
     * @brief Describes a Vertex Attribute for use in the vertex shader
     */
    struct VertexAttribute {
        //! Attribute Name in the vertex shader
        std::string name;

        //! Basic data type
        DataType type;

        //! Attribute dimension (1, 2, 3 or 4)
        int size;
    };

    /**
     * @brief Describes the structure a Vertex conforms to.
     */
    struct VertexFormat {
        //! Attributes, in order
        std::vector<VertexAttribute> attributes;
    };

    /**
     * @brief 
     */
    struct SubsetDescriptor {
        VertexFormat vertexFormat;
        std::vector<BufferDescriptor> bufferAttribMapping;
    };
}
hbg
int main(int argc, char **argv) {
    // auto screen = new XE::Graphics::Screen::New();

    // setup graphics device (An OpenGL device)
    auto graphicsDevice = new XE::Graphics::GL::GraphicsDeviceGL(/*screen*/);

    // setup a shader
    const char vertexShaderSource[] = R"(
#version 460

in vec3 vertCoord;
in vec4 vertColor;

out vec4 fragColor;

void main() {
    gl_Position = vec4(vertCoord, 1.0);
    fragColor = vertColor;
}
    )";

    const char fragmentShaderSource[] = R"(
#version 460

in vec4 fragColor;
out vec4 finalColor;

void main() {
    finalCOlor = fragColor;
}
)";

    XE::Graphics::ProgramDescriptor programDescriptor = {{
        {XE::Graphics::ShaderType::Vertex, vertexShaderSource}, 
        {XE::Graphics::ShaderType::Fragment, fragmentShaderSource}
    }};
    
    auto program = graphicsDevice->CreateProgram(programDescriptor);

    // setup a basic triangle geometry
    struct Vertex {
        XE::Math::Vector3f coord;
        XE::Math::Vector4f color;
    };
    
    std::vector<Vertex> vertices = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    };

    // create the vertex buffer
    auto vertexBuffer = graphicsDevice->CreateBuffer({
        XE::Graphics::BufferType::Vertex,
        XE::Graphics::BufferUsage::Write,
        XE::Graphics::BufferAccess::Static,
        static_cast<int>(sizeof(Vertex) * vertices.size()),
        (std::byte*)vertices.data(),
    });

    // create the index buffer
    std::vector<int> indices = {
        0, 2, 4
    };

    auto indexBuffer = graphicsDevice->CreateBuffer({
        XE::Graphics::BufferType::Index,
        XE::Graphics::BufferUsage::Write,
        XE::Graphics::BufferAccess::Static,
        static_cast<int>(sizeof(int) * indices.size()),
        (std::byte*)indices.data(),
    });

    // create a geometry subset
    XE::Graphics::SubsetDescriptor subsetDescriptor;

    // graphicsDevice->CreateSubset()

    delete graphicsDevice;

    return 0;
}
