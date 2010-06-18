# Makefile for popt library.

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I. -I$(top_srcdir)

LINT =		splint
MCCABE =	pmccabe

EXTRA_DIST = lookup3.c autogen.sh CHANGES  \
	footer_no_timestamp.html libpopt.vers \
	$(TESTS) test-poptrc\
	popt.xcodeproj/project.pbxproj \
	popt.ps Doxyfile

SUBDIRS = po . auto


noinst_HEADERS = poptint.h system.h

check_PROGRAMS  = test1 test2 tdict 

check_SCRIPTS	= $(TESTS)


test1_CPPFLAGS  = -I $(top_builddir) 
test2_CPPFLAGS  = -I $(top_builddir) 
tdict_CPPFLAGS  = -I $(top_builddir) 
test1_LDADD  = $(top_builddir)/$(usrlib_LTLIBRARIES)
test2_LDADD  = $(top_builddir)/$(usrlib_LTLIBRARIES)
tdict_LDADD  = $(top_builddir)/$(usrlib_LTLIBRARIES)

TESTS_ENVIRONMENT = top_srcdir=$(top_srcdir) PATH=.:../src:$$PATH \
       $(SHELL) 
TESTS = $(top_builddir)/testit.sh

include_HEADERS = popt.h

usrlibdir = $(libdir)
usrlib_LTLIBRARIES = libpopt.la

libpopt_la_SOURCES = popt.c poptparse.c poptconfig.c popthelp.c poptint.c
libpopt_la_LDFLAGS = -no-undefined @LTLIBINTL@ @LTLIBICONV@

pkgconfigdir = $(prefix)/lib/pkgconfig
pkgconfig_DATA = popt.pc

if HAVE_LD_VERSION_SCRIPT
libpopt_la_LDFLAGS += -Wl,--version-script=$(top_srcdir)/libpopt.vers
endif

dist_man_MANS = popt.3

BUILT_SOURCES = popt.pc # popt.lcd

distclean-local:
	rm -rf .ccache

.PHONY:	updatepo
updatepo:
	rsync -Lrtvz  translationproject.org::tp/latest/popt/  po

popt.lcd: Makefile.am ${libpopt_la_SOURCES} ${include_HEADERS} ${noinst_HEADERS}
	lclint -dump $@ ${libpopt_la_SOURCES}

.PHONY:	sources
sources:
	@echo $(libpopt_la_SOURCES:%=popt/%)

if HAVE_SPLINT
lint:
	$(LINT) ${DEFS} ${INCLUDES} test1.c ${libpopt_la_SOURCES}
endif HAVE_SPLINT 

if HAVE_MCCABE
mccabe:
	$(MCCABE) $(libpopt_la_SOURCES) | sort -n -r | head -n 10
endif HAVE_MCCABE

if DOXYGEN_ENABLE
doxygen: Doxyfile
	rm -rf doxygen
	mkdir -p doxygen
	doxygen
endif DOXYGEN_ENABLE

# run lcov from scratch, always
lcov-reset:
	make lcov-run
	make lcov-report

# run lcov from scratch if the dir is not there
lcov:
	make lcov-reset

# reset run coverage tests
lcov-run:
	@-rm -rf lcov
	find . -name "*.gcda" -exec rm {} \;
	make check

# generate report based on current coverage data
lcov-report:
	mkdir lcov
	lcov --directory . --capture --output-file lcov/lcov.info
	lcov -l lcov/lcov.info | grep -v "`cd $(top_srcdir) && pwd`" | cut -d: -f1 > lcov/remove
	lcov -r lcov/lcov.info `cat lcov/remove` > lcov/lcov.cleaned.info
	rm lcov/remove
	mv lcov/lcov.cleaned.info lcov/lcov.info
	genhtml -t "$(PACKAGE_STRING)" -o lcov lcov/lcov.info
