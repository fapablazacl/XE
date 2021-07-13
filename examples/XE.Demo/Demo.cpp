/**
 * XE Tech demo
 */

#include <XE/XE.h>
#include <XE/IO.h>
#include <XE/Input.h>
#include <XE/Math.h>
#include <XE/Graphics.h>
#include <XE/Graphics/GL.h>
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>

#include <map>

const int s_screenWidth = 1200;
const int s_screenHeight = 800;

const std::string s_simpleVS = R"(
#version 410 core

uniform mat4 uProjViewModel;

layout(location = 0) in vec3 vsCoord;
layout(location = 1) in vec4 vsColor;

out vec4 fsColor;

void main() {
    gl_Position = vec4(vsCoord, 1.0) * uProjViewModel;
    fsColor = vsColor;
}
)";


const std::string s_simpleFS = R"(
#version 410 core

in vec4 fsColor;

out vec4 finalColor;

void main() {
    finalColor = fsColor;
}
)";


struct Vertex {
    XE::Vector3f coord;
    XE::Vector4f color;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    XE::PrimitiveType primitive;
};

XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs) {
    XE::ProgramDescriptor desc;

    desc.sources = {
        { XE::ShaderType::Vertex, vs}, 
        { XE::ShaderType::Fragment, fs}, 
    };

    return desc;
}

Mesh makeIndexedCubeMesh(const float width, const float height, const float depth) {
    Mesh mesh;

    const std::vector<Vertex> vertices = {
        {{-0.5f * width, -0.5f * height,  0.5f * depth}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.5f * width, -0.5f * height,  0.5f * depth}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.5f * width,  0.5f * height,  0.5f * depth}, {0.0f, 0.0f, 1.0f, 1.0f}},
		{{0.5f * width,  0.5f * height,  0.5f * depth}, {1.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.5f * width, -0.5f * height, -0.5f * depth}, {0.0f, 1.0f, 1.0f, 1.0f}},
		{{0.5f * width, -0.5f * height, -0.5f * depth}, {1.0f, 0.0f, 1.0f, 1.0f}},
		{{-0.5f * width,  0.5f * height, -0.5f * depth}, {0.0f, 0.0f, 0.0f, 1.0f}},
		{{0.5f * width,  0.5f * height, -0.5f * depth}, {1.0f, 1.0f, 1.0f, 1.0f}}
	};

    mesh.primitive = XE::PrimitiveType::TriangleList;

    const std::vector<int> indices = {
		0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
	};

    // convertir primitiva a lista de triangulos
	bool order = true;

	for (size_t i=0; i<indices.size() - 2; i++) {
		if (order) {
			mesh.vertices.push_back(vertices[indices[i + 0]]);
			mesh.vertices.push_back(vertices[indices[i + 1]]);
			mesh.vertices.push_back(vertices[indices[i + 2]]);
		} else {
			mesh.vertices.push_back(vertices[indices[i + 0]]);
			mesh.vertices.push_back(vertices[indices[i + 2]]);
			mesh.vertices.push_back(vertices[indices[i + 1]]);
		}

		order = !order;
	}

    return mesh;
}

Mesh makeCubeMesh(const float width, const float height, const float depth) {
    Mesh mesh;

    const std::vector<Vertex> vertices = {
        {{-0.5f * width, -0.5f * height,  0.5f * depth}, {1.0f, 0.0f, 1.0f, 1.0f}},
		{{0.5f * width, -0.5f * height,  0.5f * depth}, {1.0f, 0.0f, 1.0f, 1.0f}},
		{{-0.5f * width,  0.5f * height,  0.5f * depth}, {0.0f, 1.0f, 1.0f, 1.0f}},
		{{0.5f * width,  0.5f * height,  0.5f * depth}, {0.0f, 1.0f, 1.0f, 1.0f}},
		{{-0.5f * width, -0.5f * height, -0.5f * depth}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.5f * width, -0.5f * height, -0.5f * depth}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{-0.5f * width,  0.5f * height, -0.5f * depth}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{0.5f * width,  0.5f * height, -0.5f * depth}, {0.0f, 1.0f, 0.0f, 1.0f}}
	};

    mesh.primitive = XE::PrimitiveType::TriangleList;

    const std::vector<int> indices = {
		0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
	};

    // convertir primitiva a lista de triangulos
	bool order = true;

	for (size_t i=0; i<indices.size() - 2; i++) {
		if (order) {
			mesh.vertices.push_back(vertices[indices[i + 0]]);
			mesh.vertices.push_back(vertices[indices[i + 1]]);
			mesh.vertices.push_back(vertices[indices[i + 2]]);
		} else {
			mesh.vertices.push_back(vertices[indices[i + 0]]);
			mesh.vertices.push_back(vertices[indices[i + 2]]);
			mesh.vertices.push_back(vertices[indices[i + 1]]);
		}

		order = !order;
	}

    return mesh;
}

class DemoApp {
public:
    DemoApp() {}

    int run(int argc, char **argv) {
        initialize();
        setupGeometry();

        mainLoop();

        return 0;
    }

private:
    void initialize() {
        mWindow = XE::WindowGLFW::create(XE::ContextDescriptorGL::defaultGL4(), "XE.Demo example application", {s_screenWidth, s_screenHeight}, false);
        mGraphicsDevice = std::make_unique<XE::GraphicsDeviceGL>(mWindow->getContext());
        mInputManager = mWindow->getInputManager();

        mSimpleProgram = createSimpleProgram();
    }

    XE::Program* createSimpleProgram() {
        XE::ProgramDescriptor desc = makeSimpleProgramDesc(s_simpleVS, s_simpleFS);
        XE::Program *program = mGraphicsDevice->createProgram(desc);

        return program;
    }

    void setupGeometry() {
        const Mesh mesh = makeCubeMesh(1.0f, 1.0f, 1.0f);

        mCubeSubset = createCubeSubset2(mesh);
        mCubeSubsetEnvelope = {
            mesh.primitive,
            0, 
            static_cast<int>(mesh.vertices.size())
        };

        mMaterial.renderState.depthTest = true;
        mMaterial.renderState.cullBackFace = true;
    }

    XE::Subset* createCubeSubset2(const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc {
            XE::BufferType::Vertex,
            XE::BufferUsage::Read,
            XE::BufferAccess::Static,
            static_cast<int>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const std::byte*>(mesh.vertices.data())
        };

        const XE::Buffer *vertexBuffer = mGraphicsDevice->createBuffer(bufferDesc);

        const std::vector<XE::SubsetVertexAttrib> attribs {
            {0, XE::DataType::Float32, 3, false, sizeof(Vertex), 0, 0},
            {1, XE::DataType::Float32, 4, false, sizeof(Vertex), 0, sizeof(Vertex::coord)}
        };

        const XE::SubsetDescriptor2 subsetDesc {
            &vertexBuffer, 1,
            attribs.data(), attribs.size(),
            nullptr
        };

        XE::Subset *subset = mGraphicsDevice->createSubset(subsetDesc);

        return subset;
    }

    void mainLoop() {
        while (mDone) {
            update();
            renderFrame();
        }
    }

    void update() {
        mInputManager->poll();

        auto keyboardStatus = mInputManager->getKeyboardStatus();
        if (keyboardStatus.isPressed(XE::KeyCode::KeyEsc)) {
            mDone = false;
        }

        mAngle += XE::radians(0.25f);
    }

    void renderFrame() {
        const XE::UniformMatrix uProjModelView = {
            "uProjViewModel",
            XE::DataType::Float32,
            4, 4, 1
        };

        const auto aspect = static_cast<float>(s_screenWidth) / static_cast<float>(s_screenHeight);

        const auto proj = XE::M4::perspective(XE::radians(60.0f), aspect, 0.01f, 100.0f);
        const auto view = XE::M4::lookAt({0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
        const auto model = XE::M4::translate({0.0f, 0.0f, 5.0f}) * XE::M4::rotateX(mAngle) * XE::M4::rotateY(mAngle) * XE::M4::rotateZ(mAngle);
        // const auto projViewModel = model/* * view * proj*/;

        const auto m1 = XE::M4::rotateY(mAngle);
        const auto m2 = XE::M4::translate({0.25f, 0.0f, 0.0f});
        const auto m3 = XE::M4::translate({-0.25f, 0.0f, 0.0f});

        const auto projViewModel = m3 * m1 * m2;
        
        mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 1.0f, 0);

        mGraphicsDevice->setProgram(mSimpleProgram);
        mGraphicsDevice->applyUniform(&uProjModelView, 1, reinterpret_cast<const std::byte*>(projViewModel.data()));
        mGraphicsDevice->setMaterial(&mMaterial);
        mGraphicsDevice->draw(mCubeSubset, &mCubeSubsetEnvelope, 1);

        mGraphicsDevice->endFrame();
    }

private:
    std::unique_ptr<XE::WindowGLFW> mWindow;
    std::unique_ptr<XE::GraphicsDeviceGL> mGraphicsDevice;
    XE::InputManager *mInputManager = nullptr;

    bool mDone = true;
    float mAngle = 0.0f;

    XE::Program *mSimpleProgram = nullptr;
    XE::Subset *mCubeSubset = nullptr;
    XE::SubsetEnvelope mCubeSubsetEnvelope;
    XE::Material mMaterial;
};


int main(int argc, char **argv) {
    DemoApp app;

    app.run(argc, argv);
}
