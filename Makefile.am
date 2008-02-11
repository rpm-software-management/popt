# Makefile for popt library.

AUTOMAKE_OPTIONS = 1.4 foreign

LINT = splint

EXTRA_DIST = config.rpath autogen.sh CHANGES $(man_MANS) popt.spec libpopt.vers \
	testit.sh test-poptrc test3-data/0* \
	po/*.in po/*.po po/popt.pot \
	popt.ps

SUBDIRS = po

INCLUDES = -I. -I$(top_srcdir)

noinst_HEADERS = findme.h poptint.h system.h

noinst_PROGRAMS = test1 test2 test3
test1_SOURCES = test1.c
test1_LDFLAGS = 
test1_LDADD = $(usrlib_LTLIBRARIES)
test2_SOURCES = test2.c
test2_LDFLAGS = 
test2_LDADD = $(usrlib_LTLIBRARIES)
test3_SOURCES = test3.c
test3_LDFLAGS = 
test3_LDADD = $(usrlib_LTLIBRARIES)

noinst_SCRIPTS = testit.sh

TESTS_ENVIRONMENT = \
test1="./test1"

TESTS = testit.sh

include_HEADERS = popt.h

usrlibdir = $(libdir)
usrlib_LTLIBRARIES = libpopt.la

libpopt_la_SOURCES = popt.c findme.c poptparse.c poptconfig.c popthelp.c poptint.c
libpopt_la_LDFLAGS = -no-undefined @LTLIBINTL@

if HAVE_LD_VERSION_SCRIPT
libpopt_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libpopt.vers
endif

man_MANS = popt.3

#BUILT_SOURCES = popt.lcd

popt.lcd: Makefile.am ${libpopt_la_SOURCES} ${include_HEADERS} ${noinst_HEADERS}
	lclint -dump $@ ${libpopt_la_SOURCES}

.PHONY:	sources
sources:
	@echo $(libpopt_la_SOURCES:%=popt/%)

.PHONY: lint
lint:
	$(LINT) ${DEFS} ${INCLUDES} test1.c ${libpopt_la_SOURCES}

.PHONY: doxygen
doxygen: Doxyfile
	rm -rf doxygen
	mkdir -p doxygen
	doxygen

ACLOCAL_AMFLAGS = -I m4
