#include <QtGui/QOpenGLFunctions>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShader>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtWidgets/QApplication>

class OpenGLWindow : public QOpenGLWidget, private QOpenGLFunctions
{
public:
    OpenGLWindow()
    {
        setWindowTitle("OpenGL ES 2.0, Qt6, C++");
        resize(350, 350);
    }

    void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(48.f / 255.f, 56.f / 255.f, 65.f / 255.f, 1.f);

        QString vertShaderSrc =
            "attribute vec2 aPosition;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(aPosition, 0.0, 1.0);\n"
            "}\n";

        QString fragShaderSrc =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = vec4(0.2, 0.7, 0.3, 1.0);\n"
            "}\n";

        m_program.create();
        m_program.addShaderFromSourceCode(QOpenGLShader::ShaderTypeBit::Vertex, vertShaderSrc);
        m_program.addShaderFromSourceCode(QOpenGLShader::ShaderTypeBit::Fragment, fragShaderSrc);
        m_program.link();
        m_program.bind();

        float vertPositions[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            -0.5f, 0.5f,
            0.5f, 0.5f
        };
        m_vertPosBuffer.create();
        m_vertPosBuffer.bind();
        m_vertPosBuffer.allocate(vertPositions, sizeof(vertPositions));
    }

    void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_program.bind();
        m_vertPosBuffer.bind();
        m_program.setAttributeBuffer("aPosition", GL_FLOAT, 0, 2);
        m_program.enableAttributeArray("aPosition");
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer m_vertPosBuffer;
};

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWindow w;
    w.show();
    return app.exec();
}
