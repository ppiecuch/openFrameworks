TARGET = ofx
TEMPLATE = lib
CONFIG += staticlib

QT += gui widgets network multimedia

include("ofx.pri")

CONFIG(c++11): C11 = -c11
CONFIG(debug, debug|release): DBG = dbg
else: DBG = rel

DESTDIR = $$PWD/build-$$[QMAKE_SPEC]$$C11
OBJECTS_DIR = $$DESTDIR/obj-tmp.$${DBG}
SUBDIR = $${TEMPLATE}.$${TARGET}.$${DBG}
MOC_DIR = $$DESTDIR/$$SUBDIR/ui
UI_DIR = $$DESTDIR/$$SUBDIR/ui
RCC_DIR = $$DESTDIR/$$SUBDIR/ui
