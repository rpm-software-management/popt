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
autopoint -f && autoreconf -vfi
