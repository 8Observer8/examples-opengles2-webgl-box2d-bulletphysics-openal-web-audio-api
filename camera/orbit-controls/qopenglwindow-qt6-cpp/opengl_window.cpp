#include <QtGui/QSurfaceFormat>

#include "opengl_window.h"

OpenGLWindow::OpenGLWindow()
{
    setTitle("OpenGL ES 2.0, Qt6, C++");
    resize(500, 500);

    QSurfaceFormat surfaceFormat;
    surfaceFormat.setDepthBufferSize(24);
    surfaceFormat.setSamples(4);
    setFormat(surfaceFormat);

    m_cameraController = new OrbitControls(5.f, QVector2D(30.f, 0.f), QVector2D(0.f, 0.f));
    connect(m_cameraController, &OrbitControls::update, this, &OpenGLWindow::onCameraUpdate);
}

OpenGLWindow::~OpenGLWindow()
{
    delete m_cameraController;
}

void OpenGLWindow::onCameraUpdate()
{
    m_viewMatrix = m_cameraController->getViewMatrix();
    update();
}

void OpenGLWindow::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.6f, 0.862f, 0.925f, 1.f);

    m_program.create();
    m_program.addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Vertex,
        ":/assets/shaders/color.vert");
    m_program.addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Fragment,
        ":/assets/shaders/color.frag");
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
    m_viewMatrix = m_cameraController->getViewMatrix();
}

void OpenGLWindow::resizeGL(int w, int h)
{
    m_projMatrix.setToIdentity();
    m_projMatrix.perspective(50.f, w / (float)h, 0.01f, 100.f);
    m_cameraController->resize(w, h);
}

void OpenGLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(QVector3D(0, 0, 0));
    m_modelMatrix.rotate(90, QVector3D(1, 0, 0));
    m_modelMatrix.scale(QVector3D(3, 3, 1));
    m_projViewMatrix = m_projMatrix * m_viewMatrix;
    m_mvpMatrix = m_projViewMatrix * m_modelMatrix;
    m_program.setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void OpenGLWindow::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
        case Qt::MouseButton::LeftButton:
        {
            int x = event->pos().x();
            int y = event->pos().y();
            m_cameraController->startCameraRotation(x, y);
            break;
        }
        case Qt::MouseButton::RightButton:
        {
            int x = event->pos().x();
            int y = event->pos().y();
            m_cameraController->startCameraPanning(x, y);
            break;
        }
        default:
            break;
    }
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    m_cameraController->mouseMove(x, y);
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::MouseButton::LeftButton:
        {
            m_cameraController->finishCameraRotation();
            break;
        }
        case Qt::MouseButton::RightButton:
        {
            m_cameraController->finishCameraPanning();
            break;
        }
        default:
            break;
    }
}

void OpenGLWindow::wheelEvent(QWheelEvent *event)
{
    m_cameraController->zoomInZoomOut(event->angleDelta().y());
}
