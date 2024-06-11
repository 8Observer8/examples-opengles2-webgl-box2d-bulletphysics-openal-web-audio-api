#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLWindow>
#include <QtWidgets/QApplication>

class OpenGLWindow : public QOpenGLWindow, private QOpenGLFunctions
{
public:
    OpenGLWindow()
    {
        setTitle("OpenGL ES 2.0, Qt6, C++");
        resize(350, 350);
    }

    void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(48.f / 255.f, 56.f / 255.f, 65.f / 255.f, 1.f);
    }

    void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWindow w;
    w.show();
    return app.exec();
}
