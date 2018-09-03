# Mon Jan 9 20:03:51 CET 2017
INCLUDEPATH += $$PWD/boost-deps
SOURCES += \
  "$$PWD/boost-deps/libs/filesystem/src/codecvt_error_category.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/operations.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/path.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/path_traits.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/portability.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/unique_path.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/utf8_codecvt_facet.cpp" \
  "$$PWD/boost-deps/libs/filesystem/src/windows_file_codecvt.cpp" \
  "$$PWD/boost-deps/libs/smart_ptr/src/sp_collector.cpp" \
  "$$PWD/boost-deps/libs/smart_ptr/src/sp_debug_hooks.cpp" \
  "$$PWD/boost-deps/libs/system/src/error_code.cpp" \

win32:SOURCES += \

unix:SOURCES += \

exists(/opt/local/lib/libz.a): LIBS += /opt/local/lib/libz.a
else: LIBS += -lz
exists(/opt/local/lib/libbz2.a): LIBS += /opt/local/lib/libbz2.a
else: LIBS += -lbz2
