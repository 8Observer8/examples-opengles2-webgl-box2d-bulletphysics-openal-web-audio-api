#include <QtWidgets/QApplication>

#include "opengl_window.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_UseDesktopOpenGL);
    QApplication app(argc, argv);
    OpenGLWindow w;
    w.show();
    return app.exec();
}
