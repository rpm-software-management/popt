# Build the library

# autotool compatibility stuff
add_compile_definitions(POPT_SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")
add_compile_definitions(PACKAGE="${PROJECT_NAME}")

# Setup library target
add_library(popt SHARED
	popt.c
	poptconfig.c
	popthelp.c
	poptint.c
	poptparse.c
	poptint.h
	system.h
)

target_include_directories(popt PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(popt PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${POPT_SOVERSION}
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS ON
	PUBLIC_HEADER popt.h
	LINK_FLAGS "-Wl,--no-undefined -Wl,--version-script,\"${PROJECT_SOURCE_DIR}/src/libpopt.vers\""
)

# Install the library
configure_file(${PROJECT_SOURCE_DIR}/cmake/popt.pc.in ${CMAKE_BINARY_DIR}/popt.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/popt.pc
	 DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(TARGETS popt
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
