QT += widgets
CONFIG += debug
ROOTSRC = $$PWD/../examples/3d/easyCamExample
SOURCES += \
  $$ROOTSRC/src/main.cpp \
  $$ROOTSRC/src/ofApp.cpp

HEADERS += \
  $$ROOTSRC/src/ofApp.h

RES_FILES += \

CONFIG(c++11): C11 = -c11
CONFIG(debug, debug|release): DBG = dbg
else: DBG = rel

DESTDIR = $$PWD/build-$$[QMAKE_SPEC]$$C11
OBJECTS_DIR = $$DESTDIR/obj-tmp.$${DBG}
SUBDIR = $${TEMPLATE}.$${TARGET}.$${DBG}
MOC_DIR = $$DESTDIR/$$SUBDIR/ui
UI_DIR = $$DESTDIR/$$SUBDIR/ui
RCC_DIR = $$DESTDIR/$$SUBDIR/ui
# generate qrc file from list
!isEmpty(QRC_FILE) {
  QRC_FILE=$$DESTDIR/$$SUBDIR/of-easyCamExample.qrc
  write_file($$QRC_FILE)
  for(f, RES_FILES) {
      LINE="<file></file"
      write_file($$QRC_FILE, LINE, append)
  }
}

include(ofx.pri)

ICON = of-logo.icns
RC_ICONS = of-logo.ico
