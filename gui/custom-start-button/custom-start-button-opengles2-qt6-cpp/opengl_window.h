#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <QtGui/QCloseEvent>
#include <QtGui/QMatrix4x4>
#include <QtGui/QMouseEvent>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QVector3D>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLTexture>
#include <QtOpenGL/QOpenGLWindow>

class OpenGLWindow : public QOpenGLWindow, private QOpenGLFunctions
{

public:
    OpenGLWindow();

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_projViewMatrix;
    QMatrix4x4 m_mvpMatrix;

    // QVector3D m_windowColor(0.2f, 0.2f, 0.2f);
    QVector3D m_windowColor = QVector3D(0.43f, 0.56f, 0.70f);
    QVector3D m_worldColor = QVector3D(0.43f, 0.56f, 0.70f);

    const float m_worldWidth = 200.f;
    const float m_worldHeight = 100.f;
    float m_worldAspect = m_worldHeight / m_worldWidth;
    int m_viewportX;
    int m_viewportY;
    int m_viewportWidth;
    int m_viewportHeight;

    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLBuffer m_texCoordBuffer;
    QOpenGLShaderProgram *m_pProgram;
    int m_uClickLocation;
    int m_uPickColorLocation;
    int m_uMvpMatrixLocation;

    QOpenGLTexture m_texture;
    QVector3D m_buttonPosition = QVector3D(m_worldWidth / 2.f,
        m_worldHeight / 2.f, 0.f);
    QVector3D m_buttonSize = QVector3D(60.f, 20.f, 1.f);
    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_clicked = false;
    bool m_pressed = false;
};

#endif // OPENGL_WINDOW_H
