#!/bin/sh
libtoolize=`which glibtoolize 2>/dev/null`
case $libtoolize in
/*) ;;
*)  libtoolize=`which libtoolize 2>/dev/null`
    case $libtoolize in
    /*) ;;
    *)  libtoolize=libtoolize
    esac
esac
find . -name "autom4te.cache" | xargs rm -rf 
[ ! -d m4 ]        && mkdir m4
[ ! -d build-aux ] && mkdir build-aux
autoreconf -vfi
po_dir=./po
LANG=C
ls "$po_dir"/*.po 2>/dev/null |
              sed 's|.*/||; s|\.po$||' > "$po_dir/LINGUAS"
