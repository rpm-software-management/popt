AUTOTEST =      api-sanity-autotest.pl

TDIRS =		descriptors_storage header_compile_errors test_results tests

EXTRA_DIST = types

clean-local:
	rm -rf $(TDIRS)

if HAVE_API_SANITY_AUTOTEST
check-local:
	-[ -d tests ] && ${AUTOTEST} -l popt -d desc -clean
	-${AUTOTEST} -l popt -d desc -st types -gen -build -run
endif HAVE_API_SANITY_AUTOTEST
