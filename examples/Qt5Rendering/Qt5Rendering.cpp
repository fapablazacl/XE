
#include <XE/Graphics.hpp>
#include <XE/Graphics/GL.hpp>

#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>

class XEWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    XEWidget(QWidget *parent) : QOpenGLWidget(parent) {
        deviceGL = new XE::GraphicsDeviceGL();
    }

    virtual void initializeGL() override {

    }

    virtual void resizeGL(int width, int height) override {
        deviceGL->setViewport({ {0, 0}, {width, height} });
    }

    virtual void paintGL() override {
        deviceGL->beginFrame(XE::ClearFlags::All, {0.2f, 0.2f, 0.8f, 1.0f}, 0.0f, 0);

        deviceGL->endFrame();
    }

private:
    XE::GraphicsDeviceGL *deviceGL = nullptr;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QMainWindow mainWindow;

    mainWindow.show();
    return app.exec();
}
