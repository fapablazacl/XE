
#include <XE/Math/Matrix.hpp>
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>

class XEGraphicsContextGLImpl : public XE::IGraphicsContextGL {
public:
    XEGraphicsContextGLImpl() {}

    virtual ~XEGraphicsContextGLImpl() {}

    virtual GetProcAddress getProcAddressFunction() const override {
        return nullptr;
    }

    virtual XE::IGraphicsContextGL::Descriptor getDescriptor() const override {
        return XE::IGraphicsContextGL::Descriptor::defaultGL4();
    }

    virtual void present() override {
        /*do nothing*/
    }
};

class XEWidget : public QOpenGLWidget {
public:
    XEWidget(QWidget *parent) : QOpenGLWidget(parent) {}
    
    virtual ~XEWidget() {
        if (deviceGL) {
            delete deviceGL;
        }
    }

    virtual void initializeGL() override {
        // at this point, we get a OpenGL context
        deviceGL = new XE::GraphicsDeviceGL(&context);

        const std::string vertexShaderSource = R"(
#version 410

uniform mat4 pvm;

in vec3 attribCoord;
in vec4 attribColor;

out vec4 fragColor;

void main() {
    gl_Position = pvm * vec4(attribCoord, 1.0);
    fragColor = attribColor;
}
        )";

        const std::string fragmentShaderSource = R"(
#version 410

in vec4 fragColor;

out vec4 finalColor;

void main() {
    finalColor = fragColor;
}
        )";

        const XE::ProgramDescriptor programDescriptor = {{
            {XE::ShaderType::Vertex, vertexShaderSource},
            {XE::ShaderType::Fragment, fragmentShaderSource}
        }};

        program = deviceGL->createProgram(programDescriptor);

        // setup geometry
        std::vector<XE::Vector3f> coords = {
            {0.0f, 0.5f, 0.0f},
            {0.5f, -0.5f, 0.0f},
            {-0.5f, -0.5f, 0.0f},
        };

        std::vector<XE::Vector4f> colors = {
            {1.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f, 1.0f}
        };

        XE::BufferDescriptor coordBufferDescriptor = {
            XE::BufferType::Vertex, 
            XE::BufferUsage::Copy, 
            XE::BufferAccess::Static, 
            (int)coords.size() * (int)sizeof(XE::Vector3f), 
            (const std::byte*) coords.data()
        };

        auto coordBuffer = deviceGL->createBuffer(coordBufferDescriptor);

        XE::BufferDescriptor colorBufferDescriptor = {
            XE::BufferType::Vertex, 
            XE::BufferUsage::Copy, 
            XE::BufferAccess::Static, 
            (int)colors.size() * (int)sizeof(XE::Vector4f), 
            (const std::byte*) colors.data()
        };

        auto colorBuffer = deviceGL->createBuffer(colorBufferDescriptor);

        std::vector<std::unique_ptr<XE::Buffer>> buffers;
        buffers.push_back(std::move(coordBuffer));
        buffers.push_back(std::move(colorBuffer));

        std::map<std::string, int> bufferMapping = {
            {"attribCoord", 0}, 
            {"attribColor", 1}
        };

        XE::SubsetDescriptor subsetDescriptor;
        subsetDescriptor.attributes = {
            {"attribCoord", XE::DataType::Float32, 3}, 
            {"attribColor", XE::DataType::Float32, 4},
        };

        subset = deviceGL->createSubset(subsetDescriptor, std::move(buffers), bufferMapping);
    }

    virtual void resizeGL(int width, int height) override {
        deviceGL->setViewport({ {0, 0}, {width, height} });
    }

    virtual void paintGL() override {
        XE::SubsetEnvelope envelope = {
            nullptr, 
            XE::PrimitiveType::TriangleStrip, 
            0, 3
        };

        XE::Matrix4f projViewModelMatrix = XE::Matrix4f::createIdentity();

        XE::UniformMatrix matrixLayout = { "pvm", XE::DataType::Float32, 4, 4, 1 };

        deviceGL->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 0.0f, 0);
        {
            deviceGL->setProgram(program.get());
            deviceGL->applyUniform(&matrixLayout, 1, (const std::byte*)&projViewModelMatrix);
            deviceGL->setMaterial(&material);
            deviceGL->draw(subset.get(), &envelope, 1);
        }
        deviceGL->endFrame();
    }

private:
    XE::Material material;
    XEGraphicsContextGLImpl context;
    XE::GraphicsDeviceGL *deviceGL = nullptr;

    std::unique_ptr<XE::Program> program;
    std::unique_ptr<XE::Subset> subset;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QMainWindow mainWindow;

    mainWindow.setCentralWidget(new XEWidget(&mainWindow));

    mainWindow.show();
    return app.exec();
}
