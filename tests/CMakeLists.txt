if (BUILD_SHARED_LIBS)
	set(libpopt poptShared)
else()
	set(libpopt poptStatic)
endif()

# Set up macro for building and adding tests
macro(BuildTest TESTNAME)
	add_executable(${TESTNAME} EXCLUDE_FROM_ALL ${TESTNAME}.c)
	target_link_libraries(${TESTNAME} PRIVATE ${libpopt})
	add_test(${TESTNAME}_build ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --target ${TESTNAME})
	set_tests_properties(${TESTNAME}_build PROPERTIES FIXTURES_SETUP testit_fixture)
endmacro()

set(TEST_PROGRAMS
	test1
	test2
	test3
	tdict
)

foreach (TESTPROG ${TEST_PROGRAMS})
	BuildTest(${TESTPROG})
endforeach()

configure_file(test-poptrc.in test-poptrc @ONLY)

find_program (SH_PROGRAM sh)

if (SH_PROGRAM)
	add_test (testit ${SH_PROGRAM} -c "srcdir=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testit.sh")
	set_tests_properties(testit PROPERTIES FIXTURES_REQUIRED testit_fixture)
endif (SH_PROGRAM)
