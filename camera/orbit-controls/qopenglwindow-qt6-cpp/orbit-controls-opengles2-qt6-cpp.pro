QT += core gui openglwidgets

win32: LIBS += -lopengl32

CONFIG += c++17

SOURCES += \
    main.cpp \
    opengl_window.cpp \
    orbit_controls.cpp

HEADERS += \
    opengl_window.h \
    orbit_controls.h

RESOURCES += \
    assets.qrc
