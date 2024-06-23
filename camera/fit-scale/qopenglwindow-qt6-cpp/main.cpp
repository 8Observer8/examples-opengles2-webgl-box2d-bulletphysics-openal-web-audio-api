#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QSurfaceFormat>
#include <QtGui/QVector3D>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShader>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLWindow>
#include <QtWidgets/QApplication>

class OpenGLWindow : public QOpenGLWindow, private QOpenGLFunctions
{

public:

    OpenGLWindow()
    {
        setTitle("OpenGL ES 2.0, Qt6, C++");
        resize(380, 380);

        QSurfaceFormat surfaceFormat;
        surfaceFormat.setDepthBufferSize(24);
        surfaceFormat.setSamples(4);
        setFormat(surfaceFormat);
    }

    void initializeGL() override
    {
        initializeOpenGLFunctions();
        glClearColor(0.04f, 0.62f, 0.48f, 1.f);

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
            "uniform vec3 uColor;\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = vec4(uColor, 1.0);\n"
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

        m_uColorLocation = m_program.uniformLocation("uColor");
        m_uMvpMatrixLocation = m_program.uniformLocation("uMvpMatrix");
        m_viewMatrix.lookAt(QVector3D(0, 0, 1), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
    }

    void resizeGL(int w, int h) override
    {
        int deviceW = w * devicePixelRatio();
        int deviceH = h * devicePixelRatio();
        float deviceAspect = deviceH / (float) deviceW;

        if (deviceAspect > m_worldAspect)
        {
            m_viewportWidth = deviceW;
            m_viewportHeight = (int) deviceW * m_worldAspect;
            m_viewportX = 0;
            m_viewportY = (int) (deviceH - m_viewportHeight) / 2.f;
        }
        else
        {
            m_viewportWidth = (int) deviceH / m_worldAspect;
            m_viewportHeight = deviceH;
            m_viewportX = (int) (deviceW - m_viewportWidth) / 2.f;
            m_viewportY = 0;
        }
        m_projMatrix.setToIdentity();
        m_projMatrix.ortho(0.f, m_worldWidth, 0.f, m_worldHeight, 1.f, -1.f);
        m_projViewMatrix = m_projMatrix * m_viewMatrix;
    }

    void paintGL() override
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
        glScissor(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
        glClearColor(0.04, 0.62, 0.48, 1);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        // Square
        drawRectangle(100, 50, 50, 50, 0, QVector3D(0.3, 0.07, 0.5));
        // Left border
        drawRectangle(5, 50, 5, 85, 0, QVector3D(0.62, 0.04, 0.18));
        // Right border
        drawRectangle(195, 50, 5, 85, 0, QVector3D(0.62, 0.04, 0.18));
        // Top border
        drawRectangle(100, 95, 185, 5, 0, QVector3D(0.62, 0.04, 0.18));
        // Bottom border
        drawRectangle(100, 5, 185, 5, 0, QVector3D(0.62, 0.04, 0.18));
    }

    void drawRectangle(float x, float y, float w, float h,
        float angle, const QVector3D& color)
    {
        m_modelMatrix.setToIdentity();
        m_modelMatrix.translate(QVector3D(x, y, 0));
        m_modelMatrix.rotate(angle, QVector3D(0, 0, 1));
        m_modelMatrix.scale(QVector3D(w, h, 1));
        m_mvpMatrix = m_projViewMatrix * m_modelMatrix;
        m_program.setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
        m_program.setUniformValue(m_uColorLocation, color);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

private:
    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLShaderProgram m_program;
    int m_uColorLocation;
    int m_uMvpMatrixLocation;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projViewMatrix;
    QMatrix4x4 m_modelMatrix;
    const float m_worldWidth = 200.f;
    const float m_worldHeight = 100.f;
    float m_worldAspect = m_worldHeight / m_worldWidth;
    int m_viewportX;
    int m_viewportY;
    int m_viewportWidth;
    int m_viewportHeight;
};

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWindow w;
    w.show();
    return app.exec();
}
