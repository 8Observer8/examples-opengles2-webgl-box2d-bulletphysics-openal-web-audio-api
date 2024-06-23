import sys

import numpy as np
from OpenGL.GL import (GL_COLOR_BUFFER_BIT, GL_FLOAT, GL_TRIANGLE_STRIP,
                       glClear, glClearColor, glDrawArrays)
from PyQt6.QtCore import Qt
from PyQt6.QtOpenGL import (QOpenGLBuffer, QOpenGLShader, QOpenGLShaderProgram,
                            QOpenGLWindow)
from PyQt6.QtWidgets import QApplication


class OpenGLWindow(QOpenGLWindow):

    def __init__(self):
        super().__init__()

        self.setTitle("OpenGL ES 2.0, PyQt6, Python")
        self.resize(350, 350)

    def initializeGL(self):
        glClearColor(48 / 255, 56 / 255, 65 / 255, 1)

        vertShaderSrc = """
            attribute vec2 aPosition;
            void main()
            {
                gl_Position = vec4(aPosition, 0.0, 1.0);
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

        program = QOpenGLShaderProgram(self)
        program.addShaderFromSourceCode(QOpenGLShader.ShaderTypeBit.Vertex, vertShaderSrc)
        program.addShaderFromSourceCode(QOpenGLShader.ShaderTypeBit.Fragment, fragShaderSrc)
        program.link()
        program.bind()

        vertPositions = np.array([
            -0.5, -0.5,
            0.5, -0.5,
            -0.5, 0.5,
            0.5, 0.5], dtype=np.float32)
        self.vertPosBuffer = QOpenGLBuffer()
        self.vertPosBuffer.create()
        self.vertPosBuffer.bind()
        self.vertPosBuffer.allocate(vertPositions, len(vertPositions) * 4)
        program.setAttributeBuffer("aPosition", GL_FLOAT, 0, 2)
        program.enableAttributeArray("aPosition")

    def resizeGL(self, w, h):
        pass

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)

if __name__ == "__main__":
    QApplication.setAttribute(Qt.ApplicationAttribute.AA_UseDesktopOpenGL)
    app = QApplication(sys.argv)
    w = OpenGLWindow()
    w.show()
    sys.exit(app.exec())
