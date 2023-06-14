# Build the library

# autotool compatibility stuff
add_compile_definitions(POPT_SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")
add_compile_definitions(PACKAGE="${PROJECT_NAME}")

# Setup library target

if (NOT BUILD_STATIC_LIBS AND NOT BUILD_SHARED_LIBS)
	message(FATAL_ERROR "Can't disable both shared and static libraries")
endif()

set(SOURCES
	popt.c
	poptconfig.c
	popthelp.c
	poptint.c
	poptparse.c
	poptint.h
	system.h
)

if (BUILD_SHARED_LIBS)
	add_library(poptShared SHARED ${SOURCES})
	set(shared_lib poptShared)
	target_include_directories(poptShared PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)
	if (Iconv_FOUND)
		target_link_libraries(poptShared PRIVATE Iconv::Iconv)
	endif()
	set_target_properties(poptShared PROPERTIES
		OUTPUT_NAME popt
		VERSION ${PROJECT_VERSION}
		SOVERSION ${POPT_SOVERSION}
		PUBLIC_HEADER popt.h
		LINK_FLAGS "-Wl,--no-undefined -Wl,--version-script,\"${PROJECT_SOURCE_DIR}/src/libpopt.vers\""
	)
endif()

if (BUILD_STATIC_LIBS)
	add_library(poptStatic STATIC ${SOURCES})
	set(static_lib poptStatic)
	target_include_directories(poptStatic PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	)
	set_target_properties(poptStatic PROPERTIES
		OUTPUT_NAME popt
		PUBLIC_HEADER popt.h
	)
endif()

# Install the library
configure_file(${PROJECT_SOURCE_DIR}/cmake/popt.pc.in ${CMAKE_BINARY_DIR}/popt.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/popt.pc
	 DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(TARGETS ${shared_lib} ${static_lib}
	EXPORT poptTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Generate and install the exports
export(EXPORT poptTargets
	FILE ${CMAKE_BINARY_DIR}/poptTargets.cmake
)
install(EXPORT poptTargets
	FILE poptTargets.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/popt
)
configure_package_config_file(
	${PROJECT_SOURCE_DIR}/cmake/poptConfig.cmake.in
	${CMAKE_BINARY_DIR}/poptConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/popt
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES
	${CMAKE_BINARY_DIR}/poptConfig.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/popt
)
write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/poptConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)
install(FILES
	${CMAKE_BINARY_DIR}/poptConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/popt
)
