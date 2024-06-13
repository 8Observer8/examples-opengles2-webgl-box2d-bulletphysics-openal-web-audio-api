QT += core gui openglwidgets

win32: LIBS += -lopengl32

HEADERS += \
    opengl_window.h

SOURCES += main.cpp \
    opengl_window.cpp

RESOURCES += \
    assets.qrc
