TDIRS =	descriptors_storage header_compile_errors test_results tests

EXTRA_DIST = types

clean-local:
	rm -rf $(TDIRS)

if HAVE_API_SANITY_AUTOTEST
check-local:
	-[ -d tests ] && ${API_SANITY_AUTOTEST} -l popt -d desc -clean
	-${API_SANITY_AUTOTEST} -l popt -d desc -st types -gen -build -run
endif HAVE_API_SANITY_AUTOTEST
