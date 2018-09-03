QT += multimedia

INCLUDEPATH += $$PWD/inc $$PWD/inc/boost-deps
SOURCES += $$PWD/ofx.cpp $$PWD/ofx-ext.cpp
HEADERS += $$PWD/ofx.h

include(inc/boost.pri)
