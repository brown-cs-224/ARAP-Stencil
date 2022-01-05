QT += core gui opengl

TARGET = arap
TEMPLATE = app

QMAKE_CXXFLAGS += -mstackrealign
CONFIG += c++17
CONFIG += sdk_no_version_check

unix:!macx {
    LIBS += -lGLU
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -lglu32
}

SOURCES += \
    libs/glew-1.10.0/src/glew.c \
    src/arap.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/view.cpp \
    src/viewformat.cpp \
    src/graphics/Shader.cpp \
    src/graphics/GraphicsDebug.cpp \
    src/graphics/shape.cpp \
    src/graphics/camera.cpp \
    src/graphics/MeshLoader.cpp

HEADERS += \
    src/arap.h \
    src/mainwindow.h \
    src/view.h \
    src/viewformat.h \
    src/graphics/Shader.h \
    src/graphics/ShaderAttribLocations.h \
    src/graphics/GraphicsDebug.h \
    src/graphics/shape.h \
    src/graphics/camera.h \
    ui_mainwindow.h \
    src/graphics/MeshLoader.h

FORMS += src/mainwindow.ui

RESOURCES += \
    res/shaders/shaders.qrc

DISTFILES += \
    res/shaders/anchorPoint.vert\
    res/shaders/anchorPoint.frag\
    res/shaders/anchorPoint.geom\
    res/shaders/shader.vert \
    res/shaders/shader.frag

INCLUDEPATH += src libs glm libs/glew-1.10.0/include
INCLUDEPATH += /usr/local/include/eigen3
DEPENDPATH += src libs glm libs/glew-1.10.0/include

