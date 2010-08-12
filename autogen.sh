#!/bin/sh
# $Id: autogen.sh,v 1.21 2010/08/12 16:56:37 devzero2000 Exp $
# autogen.sh: autogen.sh script for popt projects
#
# Copyright (c) 2010-2011 Elia Pinto <devzero2000@rpm5.org>
#
# This program have the same copyright notice as popt
# itself
#


# Guess whether we are using configure.ac or configure.in
if test -f configure.ac; then
  conffile="configure.ac"
elif test -f configure.in; then
  conffile="configure.in"
else
  echo "$0: could not find configure.ac or configure.in"
  exit 1
fi

# Version Used for building
# 

# Check for automake
am_version=`${AUTOMAKE:-automake} --version 2>/dev/null|sed -e 's/^[^0-9]*//;s/[a-z]* *$//;s/[- ].*//g;q'`
if test -z "$am_version"; then
 echo "$0: automake not found."
 echo "You need automake version 1.11 or newer installed"
 exit 1
fi
IFS=_; set $am_version; IFS=' '
am_version=$1
IFS=.; set $am_version; IFS=' '
# automake 1.11 or newer
if test "$1" = "1" -a "$2" -lt "11"; then
  echo "$0: automake version $am_version found."
  echo "You need automake version 1.11 or newer installed"
  exit 1
fi
# Check for autoconf
ac_version=`${AUTOCONF:-autoconf} --version 2>/dev/null|sed -e 's/^[^0-9]*//;s/[a-z]* *$//;s/[- ].*//g;q'`
if test -z "$ac_version"; then
 echo "$0: autoconf not found."
 echo "You need autoconf version 2.63 or newer installed"
 exit 1
fi
IFS=_; set $ac_version; IFS=' '
ac_version=$1
IFS=.; set $ac_version; IFS=' '
# autoconf 2.63 or newer 
if test "$1" = "2" -a "$2" -lt "63" || test "$1" -lt "2"; then
 echo "$0: autoconf version $ac_version found."
 echo "You need autoconf version 2.63 or newer installed"
 exit 1
fi
# Libtool
libtoolize=`which glibtoolize 2>/dev/null`
case $libtoolize in
		/*) ;;
		*)  libtoolize=`which libtoolize 2>/dev/null`
	case $libtoolize in
    	/*) ;;
    	*)  libtoolize=libtoolize
    	esac
esac
if test -z "$libtoolize"; then
		echo "$0: libtool not found."
fi
find . -name "autom4te.cache" | xargs rm -rf 
[ ! -d m4 ]        && mkdir m4
[ ! -d build-aux ] && mkdir build-aux
autoreconf -vfi
po_dir=./po
LANG=C
ls "$po_dir"/*.po 2>/dev/null |
              sed 's|.*/||; s|\.po$||' > "$po_dir/LINGUAS"
