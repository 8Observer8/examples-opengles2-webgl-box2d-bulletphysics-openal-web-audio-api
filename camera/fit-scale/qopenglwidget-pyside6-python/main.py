import sys

import numpy as np
from OpenGL.GL import (GL_COLOR_BUFFER_BIT, GL_FLOAT, GL_SCISSOR_TEST,
                       GL_TRIANGLE_STRIP, glClear, glClearColor, glDisable,
                       glDrawArrays, glEnable, glScissor, glViewport)
from PySide6.QtCore import Qt
from PySide6.QtGui import QMatrix4x4, QSurfaceFormat, QVector3D
from PySide6.QtOpenGL import QOpenGLBuffer, QOpenGLShader, QOpenGLShaderProgram
from PySide6.QtOpenGLWidgets import QOpenGLWidget
from PySide6.QtWidgets import QApplication


class OpenGLWidget(QOpenGLWidget):

    def __init__(self):
        super().__init__()

        self.setWindowTitle("OpenGL ES 2.0, PySide6, Python")
        self.resize(380, 380)
        self.worldWidth = 200
        self.worldHeight = 100
        self.worldAspect = self.worldHeight / self.worldWidth

        surfaceFormat = QSurfaceFormat()
        surfaceFormat.setDepthBufferSize(24)
        surfaceFormat.setSamples(4)
        self.setFormat(surfaceFormat)

    def initializeGL(self):

        vertShaderSrc = """
            attribute vec2 aPosition;
            uniform mat4 uMvpMatrix;
            void main()
            {
                gl_Position = uMvpMatrix * vec4(aPosition, 0.0, 1.0);
            }
        """

        fragShaderSrc = """
            #ifdef GL_ES
            precision mediump float;
            #endif
            uniform vec3 uColor;
            void main()
            {
                gl_FragColor = vec4(uColor, 1.0);
            }
        """

        self.program = QOpenGLShaderProgram()
        self.program.addShaderFromSourceCode(QOpenGLShader.ShaderTypeBit.Vertex, vertShaderSrc)
        self.program.addShaderFromSourceCode(QOpenGLShader.ShaderTypeBit.Fragment, fragShaderSrc)
        self.program.link()
        self.program.bind()

        vertPositions = np.array([
            -0.5, -0.5,
            0.5, -0.5,
            -0.5, 0.5,
            0.5, 0.5], dtype=np.float32)
        self.vertPosBuffer = QOpenGLBuffer()
        self.vertPosBuffer.create()
        self.vertPosBuffer.bind()
        self.vertPosBuffer.allocate(vertPositions, len(vertPositions) * 4)
        self.program.setAttributeBuffer("aPosition", GL_FLOAT, 0, 2)
        self.program.enableAttributeArray("aPosition")

        self.uColorLocation = self.program.uniformLocation("uColor")
        self.uMvpMatrixLocation = self.program.uniformLocation("uMvpMatrix")

        self.viewMatrix = QMatrix4x4()
        self.viewMatrix.lookAt(QVector3D(0, 0, 1), QVector3D(0, 0, 0),
            QVector3D(0, 1, 0))

        self.projMatrix = QMatrix4x4()
        self.projViewMatrix = QMatrix4x4()
        self.modelMatrix = QMatrix4x4()
        self.mvpMatrix = QMatrix4x4()

    def resizeGL(self, w, h):
        deviceW = w * self.devicePixelRatio()
        deviceH = h * self.devicePixelRatio()
        deviceAspect = deviceH / deviceW

        if deviceAspect > self.worldAspect:
            self.viewportWidth = int(deviceW)
            self.viewportHeight = int(deviceW * self.worldAspect)
            self.viewportX = 0
            self.viewportY = int((deviceH - self.viewportHeight) / 2)
        else:
            self.viewportWidth = int(deviceH / self.worldAspect)
            self.viewportHeight = int(deviceH)
            self.viewportX = int((deviceW - self.viewportWidth) / 2)
            self.viewportY = 0

        self.projMatrix.setToIdentity()
        self.projMatrix.ortho(0, self.worldWidth, 0, self.worldHeight, 1, -1)
        self.projViewMatrix = self.projMatrix * self.viewMatrix

    def paintGL(self):
        glClearColor(0.2, 0.2, 0.2, 1)
        glClear(GL_COLOR_BUFFER_BIT)
        glViewport(self.viewportX, self.viewportY, self.viewportWidth, self.viewportHeight)
        glScissor(self.viewportX, self.viewportY, self.viewportWidth, self.viewportHeight)
        glClearColor(0.04, 0.62, 0.48, 1)
        glEnable(GL_SCISSOR_TEST)
        glClear(GL_COLOR_BUFFER_BIT)
        glDisable(GL_SCISSOR_TEST)

        # Square
        self.drawRectangle(x = 100, y = 50, w = 50, h = 50, angle = 0,
            color = QVector3D(0.3, 0.07, 0.5))
        # Left border
        self.drawRectangle(x = 5, y = 50, w = 5, h = 85, angle = 0,
            color = QVector3D(0.62, 0.04, 0.18))
        # Right border
        self.drawRectangle(x = 195, y = 50, w = 5, h = 85, angle = 0,
            color = QVector3D(0.62, 0.04, 0.18))
        # Top border
        self.drawRectangle(x = 100, y = 95, w = 185, h = 5, angle = 0,
            color = QVector3D(0.62, 0.04, 0.18))
        # Bottom border
        self.drawRectangle(x = 100, y = 5, w = 185, h = 5, angle = 0,
            color = QVector3D(0.62, 0.04, 0.18))

    def drawRectangle(self, x, y, w, h, angle, color):
        self.modelMatrix.setToIdentity()
        self.modelMatrix.translate(QVector3D(x, y, 0))
        self.modelMatrix.rotate(angle, QVector3D(0, 0, 1))
        self.modelMatrix.scale(QVector3D(w, h, 1))
        self.mvpMatrix = self.projViewMatrix * self.modelMatrix
        self.program.setUniformValue(self.uMvpMatrixLocation, self.mvpMatrix)
        self.program.setUniformValue(self.uColorLocation, color)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)

if __name__ == "__main__":
    QApplication.setAttribute(Qt.ApplicationAttribute.AA_UseDesktopOpenGL)
    app = QApplication(sys.argv)
    w = OpenGLWidget()
    w.show()
    sys.exit(app.exec())
