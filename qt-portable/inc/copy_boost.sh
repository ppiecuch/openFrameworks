#!/bin/bash
p=""

if [ -e "/Volumes/Passport/_sdk/boost_1_59_0" ]; then
    p="/Volumes/Passport/_sdk/boost_1_59_0"
fi

if [ -e "/Volumes/USBSTORE/\ Dev/boost_1_59_0" ]; then
    p="/Volumes/USBSTORE/\ Dev/boost_1_59_0"
fi

OUT=boost-deps
PRI=boost.pri

/opt/local/bin/bcp \
	--boost="$p" \
	boost/filesystem.hpp \
	\
	system/error_code.hpp \
	system/src/error_code.cpp \
	\
	$OUT/
#
# build Qt include file:
#
echo "#" `date` > $PRI
echo "INCLUDEPATH += \$\$PWD/$OUT" >> $PRI
# exclude files that donot compile:
#   shared_ptr_helper.cpp
files=""
dedups() {
	eval local ff=\$$1
	local F=`basename $ff | sed 's/\(.*\)\..*/\1/'`
	case "$files" in
		*";$F"* )
			local DIR=`dirname "$ff"`
			local EXTENSION=${f##*.}
			local fn="$DIR/$RANDOM-`basename "$ff"`"
			echo "duplicate: $ff -> $fn"
			mv "$ff" "$fn"
			ff="$fn"
			;;
		* )
			files="$files;$F"
			;;
	esac
    eval $1="$ff"
}
if [ -e $OUT/libs ]; then
	echo "SOURCES += \\" >> $PRI
	for f in `find $OUT/libs -iname "*.cpp" ! -iname "shared_ptr_helper.cpp" \
		-path "*/src/*" ! -path "*/test/*" ! -path "*/win32/*" ! -path "*/pthread/*"|xargs`; do
		dedups f
		echo "  \"\$\$PWD/$f\" \\" >> $PRI
	done
	echo "" >> $PRI
	echo "win32:SOURCES += \\" >> $PRI
	for f in `find $OUT/libs -iname "*.cpp" ! -iname "shared_ptr_helper.cpp" \
		-path "*/src/*" ! -path "*/test/*" -path "*/win32/*"|xargs`; do
		dedups f
		echo "  \"\$\$PWD/$f\" \\" >> $PRI
	done
	echo "" >> $PRI
	echo "unix:SOURCES += \\" >> $PRI
	for f in `find $OUT/libs -iname "*.cpp" ! -iname "shared_ptr_helper.cpp" \
		-path "*/src/*" ! -path "*/test/*" -path "*/pthread/*"|xargs`; do
		dedups f
		echo "  \"\$\$PWD/$f\" \\" >> $PRI
	done
	echo "" >> $PRI
	echo "exists(/opt/local/lib/libz.a): LIBS += /opt/local/lib/libz.a" >> $PRI
	echo "else: LIBS += -lz" >> $PRI
	echo "exists(/opt/local/lib/libbz2.a): LIBS += /opt/local/lib/libbz2.a" >> $PRI
	echo "else: LIBS += -lbz2" >> $PRI
fi
wc -l $PRI
