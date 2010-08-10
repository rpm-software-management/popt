#!/bin/sh
# $Id: autogen.sh,v 1.20 2010/08/10 17:37:32 devzero2000 Exp $
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
# Check for needed features
libtool="`grep '^[ \t]*A._PROG_LIBTOOL' $conffile >/dev/null 2>&1 && echo yes || echo no`"
libtool2="`grep '^[ \t]*LT_INIT' $conffile >/dev/null 2>&1 && echo yes || echo no`"

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
if [ "$libtool" = "yes" -o "$libtool2" = "yes" ] 
then
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
fi
# Check for libtool 1.15.14 or newer if used
if test "$libtool" = "yes"; then
	lt_pversion=`${LIBTOOL:-libtool} --version 2>/dev/null|sed -e 's/([^)]*)//g;s/^[^0-9]*//;s/[- ].*//g;q'`
	if test -z "$lt_pversion"; then
 	echo "$0: libtool not found."
 	echo "You need libtool version 1.5.14 or newer installed"
	exit 1
	fi
	lt_version=`echo $lt_pversion|sed -e 's/\([a-z]*\)$/.\1/'`
	IFS=.; set $lt_version; IFS=' '
	lt_status="good"
	if test -z "$1"; then a=0 ; else a=$1;fi
	if test -z "$2"; then b=0 ; else b=$2;fi
	if test -z "$3"; then c=0 ; else c=$3;fi

	if test "$a" -lt "2"; then
   		if test "$b" -lt "5" -o "$b" =  "5" -a "$c" -lt "14" ; then
      		lt_status="bad"
   		fi
	else
    		lt_status="bad"
	fi
	if test $lt_status != "good"; then
		echo "$0: libtool version $lt_pversion found."
		echo "You need libtool version 1.5.14 or newer installed"
		exit 1
	fi
fi
# Check for libtool 2.2.6 or newer
if test "$libtool2" = "yes"; then
	lt_pversion=`${LIBTOOL:-libtool} --version 2>/dev/null|sed -e 's/([^)]*)//g;s/^[^0-9]*//;s/[- ].*//g;q'`
	if test -z "$lt_pversion"; then
 	echo "$0: libtool not found."
 	echo "You need libtool version 2.2.6 or newer installed"
	exit 1
	fi
	lt_version=`echo $lt_pversion|sed -e 's/\([a-z]*\)$/.\1/'`
	IFS=.; set $lt_version; IFS=' '
	lt_status="good"
	if test -z "$1"; then a=0 ; else a=$1;fi
	if test -z "$2"; then b=0 ; else b=$2;fi
	if test -z "$3"; then c=0 ; else c=$3;fi
	if test "$a" -le "2"; then
   		if test "$b" -lt "2" -o "$b" =  "2" -a "$c" -lt "6" ; then
      		lt_status="bad"
   		fi
	else
    		lt_status="bad"
	fi
	if test $lt_status != "good"; then
		echo "$0: libtool version $lt_pversion found."
		echo "You need libtool version 2.2.6 or newer installed"
		exit 1
	fi
fi
find . -name "autom4te.cache" | xargs rm -rf 
[ ! -d m4 ]        && mkdir m4
[ ! -d build-aux ] && mkdir build-aux
autoreconf -vfi
po_dir=./po
LANG=C
ls "$po_dir"/*.po 2>/dev/null |
              sed 's|.*/||; s|\.po$||' > "$po_dir/LINGUAS"
