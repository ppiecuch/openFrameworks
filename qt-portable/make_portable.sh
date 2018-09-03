#!/bin/bash

H=ofx.h
C=ofx.cpp
CX=ofx-ext.cpp
M=ofx.moc
HM=ofx-addon.h

echo "/* `date` */" > "$H"
echo "#pragma once" > "$H"
echo "#define TARGET_QT" > "$H"
echo "/* `date` */" > "$C"
echo "#include \"ofx.h\"" >> "$C"
echo "/* `date` */" > "$CX"


# internal libraries:
#  tessundef.c - cleanup tess library
ext="pugixml.hpp \
    json.hpp \
    utf8.h \
    tesselator.h
    ext/tess2/bucketalloc.h \
    ext/tess2/bucketalloc.c \
    ext/tess2/dict.h \
    ext/tess2/dict.c \
    ext/tess2/mesh.h \
    ext/tess2/mesh.c \
    ext/tess2/geom.h \
    ext/tess2/geom.c \
    ext/tess2/priorityq.h \
    ext/tess2/priorityq.c \
    ext/tess2/sweep.h \
    ext/tess2/sweep.c \
    ext/tess2/tess.h \
    ext/tess2/tess.c \
    ext/ft/FreeTypeAmalgam.h \
    ext/ft/FreeTypeAmalgam.c \
    ext/pugixml.cpp
"

echo "/* `date` */" > "$HM"
echo "#pragma once" > "$HM"
echo "#define TARGET_QT" > "$HM"

function process_module {
    list=addons/${1}-portable.txt
    nm=$2
    OUTC="ofx-addon-$nm.cpp"

    px=../addons/$1
    if [ ! -d "$px" ]; then
        px=../addons-all/$1
    fi
    if [ ! -d "$px" ]; then
        echo "*** $1 not found"
        return
    fi

    echo "/* `date` */" > "$OUTC"
    echo "#include \"$HM\"" >> "$OUTC"

    for f in `cat $list`; do
        ctr=${f:0:1}
        if [[ $ctr == '!' || $ctr == ':' ]]; then
            f=${f#?}
        fi

        if [ ! -f "addons/$f" ]; then
            f="$px/$f"
        else
            f="addons/$f"
        fi

        f=${f%%[[:cntrl:]]}
        EXT=${f##*.}
        FILENAME=`basename "$f" | sed 's/\(.*\)\..*/\1/'`
        DIRPATH=`dirname "$f"`

        if [ ! -f "$f" ]; then
            echo -n "_"
        elif [ ".$EXT" == ".h" -o ".$EXT" == ".hpp" -o ".$EXT" == ".inl" ]; then
            case $ctr in
            :) # private header
                echo -n ":"
                echo "" >> "$OUTC"
                echo "#line 0 \"$f\"" >> "$OUTC"
                cat $f >> "$OUTC"
                ;;
            !) # inline
                ;;
            *)
                echo -n "."
                echo "" >> "$HM"
                echo "#line 0 \"$f\"" >> "$HM"
                cat $f >> "$HM"
                ;;
            esac
        elif [ ".$EXT" == ".c" -o ".$EXT" == ".cpp" -o ".$EXT" == ".frag" -o ".$EXT" == ".vert" ]; then
            echo -n "."
            echo "" >> "$OUTC"
            echo "#line 0 \"$f\"" >> "$OUTC"
            cat $f >> "$OUTC"
        else
            echo -n "?"
        fi
    done
    echo " Ok (merge '$nm')"
}

start=`date +%s`

for f in ext/utf8.h ext/json.hpp ext/pugixml.hpp; do
    echo -n "."
    echo "" >> "$H"
    echo "#line 0 \"$f\"" >> "$H"
    cat "$f" >> "$H"
done
for f in inc/tesselator.h ext/pugixml.hpp; do
    echo -n "."
    echo "" >> "$CX"
    echo "#line 0 \"$f\"" >> "$CX"
    cat "$f" >> "$CX"
done
# process headers first
for f in $ext; do
	EXT=${f##*.}
	FNEXT=`basename $f`
	FN=`basename $f | sed 's/\(.*\)\..*/\1/'`
	DIRPATH=`dirname $f`
	if [ ! -f $f ]; then
        echo -n "_"
	elif [ ".$EXT" == ".h" -o ".$EXT" == ".hpp" ]; then
        echo -n "."
        echo "" | tee -a "$CX" >> "$C"
        echo "#line 0 \"$f\"" | tee -a "$CX" >> "$C"
        cat "$f" | tee -a "$CX" >> "$C"
	fi
done
for f in $ext; do
	EXT=${f##*.}
	FNEXT=`basename $f`
	FN=`basename $f | sed 's/\(.*\)\..*/\1/'`
	DIRPATH=`dirname $f`
	if [ ! -f $f ]; then
        echo -n "_"
	elif [ ".$EXT" == ".c" -o ".$EXT" == ".cpp" ]; then
        echo -n "."
        echo "" >> "$CX"
        echo "#line 0 \"$f\"" >> "$CX"
        cat $f >> "$CX"
	fi
done
echo " Ok (merge ext)"

# of:

for f in `cat portable.txt`; do
    ctr=${f:0:1}
    if [[ $ctr == '!' || $ctr == ':' ]]; then
        f=${f#?}
    fi
    f=${f%%[[:cntrl:]]}
	EXT=${f##*.}
	FILENAME=`basename "$f" | sed 's/\(.*\)\..*/\1/'`
	DIRPATH=`dirname "$f"`

	if [ ! -f $f ]; then
        echo -n "_"
	elif [ ".$EXT" == ".h" -o ".$EXT" == ".hpp" -o ".$EXT" == ".inl" ]; then
        case $ctr in
        :) # private header
            echo -n ":"
            echo "" >> "$C"
            echo "#line 0 \"$f\"" >> "$C"
            cat $f >> "$C"
            ;;
        !) # inline
            ;;
        *)
            echo -n "."
            echo "" >> "$H"
            echo "#line 0 \"$f\"" >> "$H"
            cat $f >> "$H"
            ;;
        esac
	elif [ ".$EXT" == ".c" -o ".$EXT" == ".cpp" -o ".$EXT" == ".frag" -o ".$EXT" == ".vert" ]; then
        echo -n "."
        echo "" >> "$C"
        echo "#line 0 \"$f\"" >> "$C"
        cat $f >> "$C"
    else
        echo -n "?"
	fi
done
for f in utf8.h json.hpp pugixml.hpp ofMain.h; do
    sed -i "" -e "s/#include \"$f\"/\/\* #include \"$f\" \*\//" "$H"
done
echo " Ok (merge portable)"
for f in $ext; do
	EXT=${f##*.}
	FNEXT=`basename $f`
	FN=`basename $f | sed 's/\(.*\)\..*/\1/'`
	DIRPATH=`dirname $f`
	if [ ".$EXT" == ".h"  -o ".$EXT" == ".hpp" ]; then
        echo -n "."
        sed -i "" \
            -e "s/#include \"$FNEXT\"/\/\* #include \"$FNEXT\" \*\//" \
            -e "s/#include \<$FNEXT\>/\/\* #include \<$FNEXT\> \*\//" \
            "$C" "$CX"
	fi
done
echo " Ok (clean ext)"

# build separate modules
CM=""
process_module ofxImGui imgui; CM="$CM ofx-addon-imgui.cpp"
process_module ofxNanoVG nanovg; CM="$CM ofx-addon-nanovg.cpp"
process_module ofxMidi midi; CM="$CM ofx-addon-midi.cpp"
process_module ofxLua lua; CM="$CM ofx-addon-lua.cpp"
process_module ofxSvg svg; CM="$CM ofx-addon-svg.cpp"

for f in `cat portable.txt`; do
    ctr=${f:0:1}
    if [[ $ctr == '!' || $ctr == ':' ]]; then
        f=${f#?}
    fi
    f=${f%%[[:cntrl:]]}
	EXT=${f##*.}
	FNEXT=`basename "$f"`
	FN=`basename "$f" | sed 's/\(.*\)\..*/\1/'`
	DIRPATH=`dirname "$f"`

    if [ $ctr == '!' ]; then
        echo -n "!" # inline
        inl=`basename "$f"`
        sed -i "" -e "/#include \"$inl\"/r $f" -e "s/#include \"$inl\"/\/\/ inline $inl/" "$C"
	elif [ ".$EXT" == ".h" -o ".$EXT" == ".hpp" -o ".$EXT" == ".inl" ]; then
        echo -n "."
	    sed -i "" \
            -e "s~#include \"$FNEXT\"~/* #include \"$FNEXT\" */~" \
            -e "s~#include \<$FNEXT\>~/* #include \<$FNEXT\> */~" \
            -e "s~#include \"\(.*\)\/$FNEXT\"~/* #include \"\1\/$FNEXT\" */~" \
            -e "s~/\* /\*~/*~" -e "s~\*/ \*/~*/~" \
            "$H"
	    sed -i "" \
            -e "s~#include \"\(.*\)\/$FNEXT\"~/* #include \"\1\/$FNEXT\" */~" \
            -e "s~#include \"$FNEXT\"~/* #include \"$FNEXT\" */~" \
            -e "s~#include \<$FNEXT\>~/* #include \<$FNEXT\> */~" \
            -e "s~#include \<\(.*\)\/$FNEXT\>~/* #include \<\1\/$FNEXT\> */~" \
            `# fix double comments (TODO: this should be resoved with regex above)` \
            -e "s~/\* /\*~/*~" -e "s~\*/ \*/~*/~" \
            "$C" "$CX" ofx-addon-svg.cpp
	fi
done
echo " Ok (clean portable)"
# remove headers from implementation:
for f in shaders\\/phong.frag shaders\\/phong.vert ft2build.h; do
    sed -i "" \
        -e "s/#include \"$f\"/\/\* #include \"$f\" \*\//" \
        -e "s/#include \<$f\>/\/\* #include \<$f\> \*\//" \
        "$C" "$CX"
done
# remove ft2 headers:
sed -i "" -e "s/^#include \(FT_.*\)/\/\* #include \1 \*\//" "$C" "$CX"
# remove ofMain.h:
sed -i "" -e "s/^#include \(\"ofMain.h\"\)/\/\* #include \1 \*\//" "$C" "$CX"

echo "#include \"$M\"" >> "$C"
echo ""

end=`date +%s`
echo "Done in $((end-start)) sec."
