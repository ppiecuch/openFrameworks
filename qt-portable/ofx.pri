QT += multimedia

INCLUDEPATH += $$PWD/inc $$PWD/inc/boost-deps
SOURCES += $$PWD/ofMain.cpp $$PWD/ofMainExt.cpp
SOURCES += $$PWD/ofxNanoVG.cpp $$PWD/ofxImGui.cpp
HEADERS += $$PWD/ofMain.h

DEFINES += NANOVG_GL3_IMPLEMENTATION IMGUI_DEFINE_MATH_OPERATORS

include(inc/boost.pri)
