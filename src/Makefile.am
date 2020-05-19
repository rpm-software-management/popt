# Makefile for popt library.

EXTRA_DIST = lookup3.c libpopt.vers

AM_CPPFLAGS = -I. -I$(top_srcdir)
AM_CPPFLAGS += -DPOPT_SYSCONFDIR="\"$(sysconfdir)\""

noinst_HEADERS = poptint.h system.h

include_HEADERS = popt.h
lib_LTLIBRARIES = libpopt.la

libpopt_la_SOURCES = popt.c poptparse.c poptconfig.c popthelp.c poptint.c

libpopt_la_LDFLAGS = -no-undefined @LTLIBINTL@ @LTLIBICONV@
# libtool current:revision:age info
libpopt_la_LDFLAGS += -version-info 0:1:0

if HAVE_LD_VERSION_SCRIPT
libpopt_la_LDFLAGS += -Wl,--version-script=$(top_srcdir)/src/libpopt.vers
endif

