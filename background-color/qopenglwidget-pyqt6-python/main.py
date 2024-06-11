import sys

from OpenGL.GL import GL_COLOR_BUFFER_BIT, glClear, glClearColor
from PyQt6.QtCore import Qt
from PyQt6.QtOpenGLWidgets import QOpenGLWidget
from PyQt6.QtWidgets import QApplication


class OpenGLWidget(QOpenGLWidget):

    def __init__(self):
        super().__init__()

        self.setWindowTitle("OpenGL ES 2.0, PyQt6, Python")
        self.resize(350, 350)

    def initializeGL(self):
        glClearColor(48 / 255, 56 / 255, 65 / 255, 1)

    def resizeGL(self, w, h):
        pass

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT)

if __name__ == "__main__":
    QApplication.setAttribute(Qt.ApplicationAttribute.AA_UseDesktopOpenGL)
    app = QApplication(sys.argv)
    w = OpenGLWidget()
    w.show()
    sys.exit(app.exec())
