QT += core gui openglwidgets

win32: LIBS += -lopengl32

CONFIG += c++17

SOURCES += \
    main.cpp
