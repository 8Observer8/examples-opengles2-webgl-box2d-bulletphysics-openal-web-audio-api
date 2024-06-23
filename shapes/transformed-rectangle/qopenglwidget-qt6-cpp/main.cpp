#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QVector3D>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShader>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtWidgets/QApplication>

class OpenGLWidget : public QOpenGLWidget, private QOpenGLFunctions
{

public:

    OpenGLWidget()
    {
        setWindowTitle("OpenGL ES 2.0, Qt6, C++");
        resize(380, 380);

        QSurfaceFormat surfaceFormat;
        surfaceFormat.setDepthBufferSize(24);
        surfaceFormat.setSamples(4);
        setFormat(surfaceFormat);
    }

    void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(0.188f, 0.22f, 0.255f, 1.f);

        QString vertShaderSrc =
            "attribute vec2 aPosition;\n"
            "uniform mat4 uMvpMatrix;"
            "void main()\n"
            "{\n"
            "    gl_Position = uMvpMatrix * vec4(aPosition, 0.0, 1.0);\n"
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
        m_program.setAttributeBuffer("aPosition", GL_FLOAT, 0, 2);
        m_program.enableAttributeArray("aPosition");

        m_uMvpMatrixLocation = m_program.uniformLocation("uMvpMatrix");
        m_viewMatrix.lookAt(QVector3D(0, 0, 1), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    }

    void resizeGL(int w, int h) override
    {
        const float coofWidth = w / (float)m_initialWindowWidth;
        const float coofHeight = h / (float)m_initialWindowHeight;

        m_projMatrix.setToIdentity();
        m_projMatrix.ortho(0.f, m_worldWidth * coofWidth,
            0.f, m_worldHeight * coofHeight, 1.f, -1.f);
        m_projViewMatrix = m_projMatrix * m_viewMatrix;
    }

    void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_modelMatrix.setToIdentity();
        m_modelMatrix.translate(QVector3D(50, 50, 0));
        m_modelMatrix.rotate(10, QVector3D(0, 0, 1));
        m_modelMatrix.scale(QVector3D(80, 10, 1));
        m_mvpMatrix = m_projViewMatrix * m_modelMatrix;
        m_program.setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:

    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLShaderProgram m_program;
    int m_uMvpMatrixLocation;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projViewMatrix;
    QMatrix4x4 m_modelMatrix;
    const int m_initialWindowWidth = 380;
    const int m_initialWindowHeight = 380;
    const float m_worldWidth = 100.f;
    const float m_worldHeight = 100.f;
};

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWidget w;
    w.show();
    return app.exec();
}
