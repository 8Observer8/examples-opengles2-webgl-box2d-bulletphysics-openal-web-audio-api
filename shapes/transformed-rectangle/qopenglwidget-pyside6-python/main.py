import sys

import numpy as np
from OpenGL.GL import (GL_COLOR_BUFFER_BIT, GL_FLOAT, GL_TRIANGLE_STRIP,
                       glClear, glClearColor, glDrawArrays)
from PySide6.QtCore import Qt
from PySide6.QtGui import QMatrix4x4, QSurfaceFormat, QVector3D
from PySide6.QtOpenGL import QOpenGLBuffer, QOpenGLShader, QOpenGLShaderProgram
from PySide6.QtOpenGLWidgets import QOpenGLWidget
from PySide6.QtWidgets import QApplication


class OpenGLWidget(QOpenGLWidget):

    def __init__(self):
        super().__init__()

        self.setWindowTitle("OpenGL ES 2.0, PySide6, Python")
        self.initialWindowWidth = 380
        self.initialWindowHeight = 380
        self.resize(self.initialWindowWidth, self.initialWindowHeight)
        self.worldWidth = 100
        self.worldHeight = 100

        surfaceFormat = QSurfaceFormat()
        surfaceFormat.setDepthBufferSize(24)
        surfaceFormat.setSamples(4)
        self.setFormat(surfaceFormat)

    def initializeGL(self):
        glClearColor(0.188, 0.22, 0.255, 1)

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
            void main()
            {
                gl_FragColor = vec4(0.2, 0.7, 0.3, 1.0);
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

        self.uMvpMatrixLocation = self.program.uniformLocation("uMvpMatrix")

        self.viewMatrix = QMatrix4x4()
        self.viewMatrix.lookAt(QVector3D(0, 0, 1), QVector3D(0, 0, 0),
            QVector3D(0, 1, 0))

        self.projMatrix = QMatrix4x4()
        self.projViewMatrix = QMatrix4x4()
        self.modelMatrix = QMatrix4x4()
        self.mvpMatrix = QMatrix4x4()

    def resizeGL(self, w, h):
        coofWidth = w / self.initialWindowWidth
        coofHeight = h / self.initialWindowHeight

        self.projMatrix.setToIdentity()
        self.projMatrix.ortho(0, self.worldWidth * coofWidth,
            0, self.worldHeight * coofHeight, 1, -1)
        self.projViewMatrix = self.projMatrix * self.viewMatrix

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        self.modelMatrix.setToIdentity()
        self.modelMatrix.translate(QVector3D(50, 50, 0))
        self.modelMatrix.rotate(10, QVector3D(0, 0, 1))
        self.modelMatrix.scale(QVector3D(80, 10, 1))
        self.mvpMatrix = self.projViewMatrix * self.modelMatrix
        self.program.setUniformValue(self.uMvpMatrixLocation, self.mvpMatrix)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)

if __name__ == "__main__":
    QApplication.setAttribute(Qt.ApplicationAttribute.AA_UseDesktopOpenGL)
    app = QApplication(sys.argv)
    w = OpenGLWidget()
    w.show()
    sys.exit(app.exec())
