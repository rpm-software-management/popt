# Makefile for popt library.

EXTRA_DIST = testit.sh test-poptrc

AM_CPPFLAGS = -I. -I$(top_srcdir)/src

noinst_PROGRAMS = test1 test2 tdict # test3
test1_SOURCES = test1.c
test1_LDFLAGS = 
test1_LDADD = $(top_builddir)/src/libpopt.la
test2_SOURCES = test2.c
test2_LDFLAGS = 
test2_LDADD = $(top_builddir)/src/libpopt.la
#test3_SOURCES = test3.c
#test3_LDFLAGS = 
#ltest3_LDADD = $(top_builddir)/src/libpopt.la
tdict_SOURCES = tdict.c
tdict_LDFLAGS = 
tdict_LDADD = $(top_builddir)/src/libpopt.la

noinst_SCRIPTS = testit.sh

TESTS_ENVIRONMENT = \
test1="test1"

TESTS = testit.sh

distclean-local:
	rm -rf .ccache

