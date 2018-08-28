#!/bin/bash

src=$1
if [ -z "$src" ]; then
    echo "$0 <example dir. location>"
else
    echo "reading $src"
    base=of-`basename $src | sed 's/\(.*\)\..*/\1/'`
    pro=${base}.pro
    root=`echo "$src"|sed "s#$HOME#\$\(HOME\)#"`
    echo "creating $pro"
    echo "# `date`" > $pro
    echo "QT += widgets" > $pro
    echo "ROOTSRC = $root" >> $pro
    echo "SOURCES += \\" >> $pro
    for f in `cd $src;find src -type f -iname "*.cpp" -o -iname "*.c"|xargs`; do
        echo "  \$\$ROOTSRC/$f \\" >> $pro
    done
    echo "" >> $pro
    echo "HEADERS += \\" >> $pro
    for f in `cd $src;find src -type f -iname "*.h"|xargs`; do
        echo "  \$\$ROOTSRC/$f \\" >> $pro
    done
    echo "" >> $pro
    echo "RES_FILES += \\" >> $pro
    for f in `cd $src;find bin -type f -not -name ".*"|xargs`; do
        echo "  \$\$ROOTSRC/$f \\" >> $pro
    done
    echo "" >> $pro
    echo "CONFIG(c++11): C11 = -c11" >> $pro
    echo "CONFIG(debug, debug|release): DBG = dbg" >> $pro
    echo "else: DBG = rel" >> $pro
    echo "" >> $pro
    echo "DESTDIR = \$\$PWD/build-\$\$[QMAKE_SPEC]\$\$C11" >> $pro
    echo "SUBDIR = \$\${TEMPLATE}\$\${TARGET}.\$\${DBG}" >> $pro
    echo "OBJECTS_DIR = \$\$DESTDIR/\$\$SUBDIR/obj" >> $pro
    echo "MOC_DIR = \$\$DESTDIR/\$\$SUBDIR/ui" >> $pro
    echo "UI_DIR = \$\$DESTDIR/\$\$SUBDIR/ui" >> $pro
    echo "RCC_DIR = \$\$DESTDIR/\$\$SUBDIR/ui" >> $pro
    echo "# generate qrc file from list" >> $pro
    echo "!isEmpty(QRC_FILE) {" >> $pro
    echo "  QRC_FILE=\$\$DESTDIR/\$\$SUBDIR/$base.qrc" >> $pro
    echo "  write_file(\$\$QRC_FILE)" >> $pro
    echo "  for(f, RES_FILES) {" >> $pro
        echo "      LINE=\"<file></file\"" >> $pro
        echo "      write_file(\$\$QRC_FILE, LINE, append)" >> $pro
    echo "  }" >> $pro
    echo "}" >> $pro
    echo "" >> $pro
    echo "include(ofx.pri)" >> $pro
fi
