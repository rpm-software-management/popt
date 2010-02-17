AUTOMAKE_OPTIONS = 1.4 foreign

AUTOTEST =      api-sanity-autotest.pl

TDIRS =		descriptors_storage header_compile_errors test_results tests

clean-local:
	rm -rf $(TDIRS)

check-local:
	-[ -d tests ] && ${AUTOTEST} -l popt -d popt.desc -clean
	-${AUTOTEST} -l popt -d popt.desc -st popt.types -gen -build -run
