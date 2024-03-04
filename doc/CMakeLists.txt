# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
	if(NOT DOXYGEN_FOUND)
		message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()

	set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
	if (ENABLE_WERROR)
		set(DOXYGEN_WARN_AS_ERROR YES)
	endif()
	doxygen_add_docs(apidocs
		${CMAKE_SOURCE_DIR}/src/popt.h
		ALL USE_STAMP_FILE
	)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html TYPE DOC)
endif()

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../popt.3
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man3)

install(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/../COPYING
	${CMAKE_CURRENT_SOURCE_DIR}/../CREDITS
	${CMAKE_CURRENT_SOURCE_DIR}/../README
	TYPE DOC)
