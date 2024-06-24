#ifndef ORBIT_CONTROLS_H
#define ORBIT_CONTROLS_H

#include <QtCore/QObject>
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>

class OrbitControls : public QObject
{
    Q_OBJECT

public:
    OrbitControls(float viewDistance, const QVector2D &cameraRotationXY,
        const QVector2D &cameraPanXY);

    void startCameraRotation(int x, int y);
    void finishCameraRotation();

    void startCameraPanning(int x, int y);
    void finishCameraPanning();

    void mouseMove(int x, int y);
    void zoomInZoomOut(float angleDeltaY);

    QMatrix4x4 getViewMatrix();
    void resize(int width, int height);

signals:
    void update();

private:
    float m_viewDistance;
    QVector2D m_cameraRotationXY;
    QVector2D m_cameraPanXY;
    bool m_mouseHolding = false;
    int m_mousePrevRotX = 0;
    int m_mousePrevRotY = 0;
    float m_degreesPerPixelX;
    float m_degreesPerPixelY;

    bool m_mousePanning = false;
    int m_mousePrevPanX = 0;
    int m_mousePrevPanY = 0;
    QVector3D m_center;
};

#endif // ORBIT_CONTROLS_H
