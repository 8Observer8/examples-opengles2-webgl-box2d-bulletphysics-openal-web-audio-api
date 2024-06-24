#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QVector3D>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtOpenGL/QOpenGLBuffer>
#include <QtOpenGL/QOpenGLShader>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLWindow>

#include "orbit_controls.h"

class OpenGLWindow : public QOpenGLWindow, private QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWindow();
    ~OpenGLWindow();

private slots:
    void onCameraUpdate();

private:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QOpenGLBuffer m_vertPosBuffer;
    QOpenGLShaderProgram m_program;
    int m_uMvpMatrixLocation;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_projMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projViewMatrix;
    QMatrix4x4 m_modelMatrix;
    OrbitControls *m_cameraController;
};

#endif // OPENGL_WINDOW_H
