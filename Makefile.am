# Makefile for popt library.

MCCABE =	pmccabe

EXTRA_DIST = autogen.sh CHANGES $(man_MANS) ci/Dockerfile build-aux popt.pdf

SUBDIRS = src po tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = popt.pc

man_MANS = popt.3

BUILT_SOURCES = popt.pc # popt.lcd

.PHONY: ci
ci:
	podman build -t popt -f ci/Dockerfile .
	podman run -t popt

.PHONY:	updatepo
updatepo:
	rsync -Lrtvz  translationproject.org::tp/latest/popt/  po

popt.lcd: Makefile.am ${libpopt_la_SOURCES} ${include_HEADERS} ${noinst_HEADERS}
	lclint -dump $@ ${libpopt_la_SOURCES}

.PHONY:	sources
sources:
	@echo $(libpopt_la_SOURCES:%=popt/%)

.PHONY:	mccabe
mccabe:
	$(MCCABE) $(libpopt_la_SOURCES) | sort -n -r | head -n 10

.PHONY: doxygen
doxygen: Doxyfile
	rm -rf doxygen
	mkdir -p doxygen
	doxygen

.PHONY:	lcov-reset	# run lcov from scratch, always
lcov-reset:
	make lcov-run
	make lcov-report

.PHONY:	lcov		# run lcov from scratch if the dir is not there
lcov:
	make lcov-reset

.PHONY:	lcov-run	# reset run coverage tests
lcov-run:
	@-rm -rf lcov
	find . -name "*.gcda" -exec rm {} \;
	make check

.PHONY:	lcov-report	# generate report based on current coverage data
lcov-report:
	mkdir lcov
	lcov --directory . --capture --output-file lcov/lcov.info
	lcov -l lcov/lcov.info | grep -v "`cd $(top_srcdir) && pwd`" | cut -d: -f1 > lcov/remove
	lcov -r lcov/lcov.info `cat lcov/remove` > lcov/lcov.cleaned.info
	rm lcov/remove
	mv lcov/lcov.cleaned.info lcov/lcov.info
	genhtml -t "$(PACKAGE_STRING)" -o lcov lcov/lcov.info

#.PHONY:	lcov-upload
#lcov-upload: lcov
#	rsync -rvz -e ssh --delete lcov/* ???

ACLOCAL_AMFLAGS = -I m4
