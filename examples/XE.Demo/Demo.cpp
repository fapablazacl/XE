/**
 * XE Tech demo
 */

#include <XE/XE.h>
#include <XE/IO.h>
#include <XE/Input.h>
#include <XE/Graphics.h>
#include <XE/Graphics/GL.h>
#include <XE/Graphics/GL/GLFW/WindowGLFW.h>

#include <map>

const std::string s_simpleVS = R"(
#version 410 core

in vec3 vsCoord;
in vec4 vsColor;

out vec4 fsColor;

void main() {
    gl_Position = vec4(vsCoord, 1.0);
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
    XE::PrimitiveType primitive;
};

XE::ProgramDescriptor makeSimpleProgramDesc(const std::string &vs, const std::string &fs) {
    XE::ProgramDescriptor desc;

    desc.sources = {
        { XE::ShaderType::Vertex,  vs}, 
        { XE::ShaderType::Fragment,  fs}, 
    };

    return desc;
}


Mesh makeCubeMesh(const float width, const float height, const float depth) {
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
        mWindow = XE::WindowGLFW::create(XE::ContextDescriptorGL::defaultGL4(), "XE.Demo example application", {1200, 800}, false);
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

        mCubeSubset = createCubeSubset(mesh);
        mCubeSubsetEnvelope = {
            nullptr,
            mesh.primitive,
            0, 
            static_cast<int>(mesh.vertices.size())
        };

        mMaterial.renderState.depthTest = true;
    }

    XE::Subset* createCubeSubset(const Mesh &mesh) {
        const XE::BufferDescriptor bufferDesc = {
            XE::BufferType::Vertex,
            XE::BufferUsage::Read,
            XE::BufferAccess::Static,
            static_cast<int>(sizeof(Vertex) * mesh.vertices.size()),
            reinterpret_cast<const std::byte*>(mesh.vertices.data())
        };

        XE::Buffer *vertexBuffer = mGraphicsDevice->createBuffer(bufferDesc);

        const XE::SubsetDescriptor subsetDesc = {
            {
                XE::VertexAttribute{"vsCoord", XE::DataType::Float32, 3},
                XE::VertexAttribute{"vsColor", XE::DataType::Float32, 4}
            },
            XE::DataType::Unknown
        };

        const std::map<std::string, int> bufferMapping = {
            {"vsCoord", 0},
            {"vsColor", 0}
        };

        XE::Subset *subset = mGraphicsDevice->createSubset(subsetDesc, {vertexBuffer}, bufferMapping, nullptr);

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
        if (keyboardStatus.getState(XE::KeyCode::KeyEsc) == XE::BinaryState::Press) {
            mDone = false;
        }
    }

    void renderFrame() {
        mGraphicsDevice->beginFrame(XE::ClearFlags::All, {0.0f, 0.0f, 1.0f, 0.0f}, 0.0f, 0);

        mGraphicsDevice->setProgram(mSimpleProgram);
        mGraphicsDevice->setMaterial(&mMaterial);
        mGraphicsDevice->draw(mCubeSubset, &mCubeSubsetEnvelope, 1);

        mGraphicsDevice->endFrame();
    }

private:
    std::unique_ptr<XE::WindowGLFW> mWindow;
    std::unique_ptr<XE::GraphicsDeviceGL> mGraphicsDevice;
    XE::InputManager *mInputManager = nullptr;

    bool mDone = true;

    XE::Program *mSimpleProgram = nullptr;
    XE::Subset *mCubeSubset = nullptr;
    XE::SubsetEnvelope mCubeSubsetEnvelope;
    XE::Material mMaterial;
};


int main(int argc, char **argv) {
    DemoApp app;

    app.run(argc, argv);
}
