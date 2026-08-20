# Makefile for popt library.

EXTRA_DIST = testit.sh \
             test-poptrc \
             test3-data/01.answer \
             test3-data/01.input \
             test3-data/02.answer \
             test3-data/02.input \
             test3-data/03.answer \
             test3-data/03.input

AM_CPPFLAGS = -I. -I$(top_srcdir)/src

noinst_PROGRAMS = test1 test2 tdict test3 tstuff
noinst_PROGRAMS += test4-1 test4-2 test4-3 test4-4
test1_SOURCES = test1.c
test1_CFLAGS = -Wno-discarded-qualifiers
test1_LDFLAGS = 
test1_LDADD = $(top_builddir)/src/libpopt.la
test2_SOURCES = test2.c
test2_LDFLAGS = 
test2_LDADD = $(top_builddir)/src/libpopt.la
test3_SOURCES = test3.c
test3_LDFLAGS = 
test3_LDADD = $(top_builddir)/src/libpopt.la
test4_1_SOURCES = test4-1.c
test4_1_LDFLAGS = 
test4_1_LDADD = $(top_builddir)/src/libpopt.la
test4_2_SOURCES = test4-2.c
test4_2_LDFLAGS = 
test4_2_LDADD = $(top_builddir)/src/libpopt.la
test4_3_SOURCES = test4-3.c
test4_3_LDFLAGS = 
test4_3_LDADD = $(top_builddir)/src/libpopt.la
test4_4_SOURCES = test4-4.c
test4_4_LDFLAGS = 
test4_4_LDADD = $(top_builddir)/src/libpopt.la
tdict_SOURCES = tdict.c
tdict_LDFLAGS = 
tdict_LDADD = $(top_builddir)/src/libpopt.la
tstuff_SOURCES = tstuff.c
tstuff_LDFLAGS = 
tstuff_LDADD = $(top_builddir)/src/libpopt.la

noinst_SCRIPTS = testit.sh

TESTS_ENVIRONMENT = \
test1="test1"

TESTS = testit.sh

distclean-local:
	rm -rf .ccache

