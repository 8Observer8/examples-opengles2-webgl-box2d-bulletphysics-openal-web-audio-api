#include "opengl_window.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtGui/QSurfaceFormat>

OpenGLWindow::OpenGLWindow()
    : m_texture(QOpenGLTexture::Target::Target2D)
{
    setTitle("OpenGL ES 2.0, Qt6, C++");

#ifndef Q_OS_WASM
    resize(600, 300);
#endif // Q_OS_WASM

#ifdef _WIN32
    PostMessage(GetForegroundWindow(), WM_INPUTLANGCHANGEREQUEST, 1, 0x04090409);
#endif

    // Set format
    // QSurfaceFormat format;
    // format.setSamples(4);
    // format.setSwapInterval(0);
    // connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
    // setFormat(format);

    m_viewMatrix.lookAt(QVector3D(0, 0, 1), QVector3D(0, 0, 0),
        QVector3D(0, 1, 0));
}

void OpenGLWindow::initializeGL()
{
        initializeOpenGLFunctions();

        // glClearColor(0.77, 0.64, 0.52, 1); // Light brown

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_pProgram = new QOpenGLShaderProgram(this);
        m_pProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Vertex,
            ":/assets/shaders/texture.vert");
        m_pProgram->addShaderFromSourceFile(QOpenGLShader::ShaderTypeBit::Fragment,
            ":/assets/shaders/texture.frag");
        m_pProgram->link();
        m_pProgram->bind();

        m_uClickLocation = m_pProgram->uniformLocation("uClick");
        m_uPickColorLocation = m_pProgram->uniformLocation("uPickColor");
        m_uMvpMatrixLocation = m_pProgram->uniformLocation("uMvpMatrix");
        m_pProgram->setUniformValue(m_uClickLocation, false);

        QFile file(":/assets/textures/button.json");
        QString content;
        if (file.open(QIODevice::OpenModeFlag::ReadOnly))
        {
            content = file.readAll();
        }
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
        QJsonObject root = doc.object();
        // Width and height
        QJsonValue meta = root.value("meta");
        QJsonValue size = meta.toObject().value("size");
        float tw = size.toObject().value("w").toDouble();
        float th = size.toObject().value("h").toDouble();
        // Frames
        QJsonValue frames = root.value("frames");
        // Button normal
        QJsonValue buttonActive = frames.toObject().value("button-normal.png");
        QJsonValue frame1 = buttonActive.toObject().value("frame");
        float f1x = frame1.toObject().value("x").toDouble();
        float f1y = frame1.toObject().value("y").toDouble();
        float f1w = frame1.toObject().value("w").toDouble();
        float f1h = frame1.toObject().value("h").toDouble();
        // Button active
        QJsonValue buttonNormal = frames.toObject().value("button-active.png");
        QJsonValue frame2 = buttonNormal.toObject().value("frame");
        float f2x = frame2.toObject().value("x").toDouble();
        float f2y = frame2.toObject().value("y").toDouble();
        float f2w = frame2.toObject().value("w").toDouble();
        float f2h = frame2.toObject().value("h").toDouble();

        float vertPositions[] = {
            -0.5, -0.5,
            0.5, -0.5,
            -0.5, 0.5,
            0.5, 0.5,
            -0.5, -0.5,
            0.5, -0.5,
            -0.5, 0.5,
            0.5, 0.5
        };
        m_vertPosBuffer.create();
        m_vertPosBuffer.bind();
        m_vertPosBuffer.allocate(vertPositions, sizeof(vertPositions));
        int aPositionLocation = m_pProgram->attributeLocation("aPosition");
        m_pProgram->setAttributeBuffer(aPositionLocation, GL_FLOAT, 0, 2);
        m_pProgram->enableAttributeArray(aPositionLocation);

        float texCoords[] = {
            f1x / tw, (f1y + f1h) / th, // First button texture
            (f1x + f1w) / tw, (f1y + f1h) / th,
            f1x / tw, f1y / th,
            (f1x + f1w) / tw, f1y / th,
            f2x / tw, (f2y + f2h) / th, // Second button texture
            (f2x + f2w) / tw, (f2y + f2h) / th,
            f2x / tw, f2y / th,
            (f2x + f2w) / tw, f2y / th
        };
        m_texCoordBuffer.create();
        m_texCoordBuffer.bind();
        m_texCoordBuffer.allocate(texCoords, sizeof(texCoords));
        int aTexCoordLocation = m_pProgram->attributeLocation("aTexCoord");
        m_pProgram->setAttributeBuffer(aTexCoordLocation, GL_FLOAT, 0, 2);
        m_pProgram->enableAttributeArray(aTexCoordLocation);

        m_texture.create();
        m_texture.setData(QImage(":/assets/textures/button.png"));
        m_texture.setMinMagFilters(QOpenGLTexture::Filter::Linear,
            QOpenGLTexture::Filter::Linear);
        m_texture.setWrapMode(QOpenGLTexture::WrapMode::ClampToEdge);
}

void OpenGLWindow::resizeGL(int w, int h)
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

void OpenGLWindow::paintGL()
{
    if (m_clicked)
    {
        m_clicked = false;

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(m_windowColor.x(), m_windowColor.y(), m_windowColor.z(), 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
        glScissor(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
        glClearColor(m_worldColor.x(), m_worldColor.y(), m_worldColor.z(), 1.f);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        m_pProgram->bind();

        m_pProgram->setUniformValue(m_uClickLocation, true);
        m_pProgram->setUniformValue(m_uPickColorLocation, QVector3D(1, 0, 0));

        m_modelMatrix.setToIdentity();
        m_modelMatrix.translate(m_buttonPosition);
        m_modelMatrix.scale(m_buttonSize);
        m_mvpMatrix = m_projViewMatrix * m_modelMatrix;
        m_pProgram->setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        GLubyte pixels[3];
        glReadPixels(m_mouseX, m_mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        GLubyte r = pixels[0];
        GLubyte g = pixels[1];
        GLubyte b = pixels[2];
        qDebug() << r << g << b;
        // qDebug() << m_mouseX << m_mouseY;
        if (r == 255 && g == 0 && b == 0)
        {
            qDebug() << "clicked";
            m_pressed = true;
        }

        m_pProgram->setUniformValue(m_uClickLocation, false);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(m_windowColor.x(), m_windowColor.y(), m_windowColor.z(), 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
    glScissor(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
    glClearColor(m_worldColor.x(), m_worldColor.y(), m_worldColor.z(), 1.f);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    m_pProgram->bind();
    m_texture.bind();

    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(m_buttonPosition);
    m_modelMatrix.scale(m_buttonSize);
    m_mvpMatrix = m_projViewMatrix * m_modelMatrix;
    m_pProgram->setUniformValue(m_uMvpMatrixLocation, m_mvpMatrix);
    if (!m_pressed)
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    else
    {
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    }
}

void OpenGLWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseX = event->pos().x() * devicePixelRatio();
        m_mouseY = (height() - event->pos().y() - 1) * devicePixelRatio();
        m_clicked = true;
        update();
    }
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_pressed = false;
    update();
}

void OpenGLWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_texture.destroy();
}
