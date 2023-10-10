# Build the library
# Include functionality to automatically generate export header files for shared libraries
include(GenerateExportHeader)

# autotool compatibility stuff
add_compile_definitions(POPT_SYSCONFDIR="${CMAKE_INSTALL_FULL_SYSCONFDIR}")
add_compile_definitions(PACKAGE="${PROJECT_NAME}")

# Source library target
set(POTP_SOURCE
	popt.c
	poptconfig.c
	popthelp.c
	poptint.c
	poptparse.c
	poptint.h
	system.h
	system.c
)

# Setup library target
if (BUILD_SHARED_LIBS)
	add_library(popt SHARED ${POTP_SOURCE})
else ()
	add_library(popt STATIC ${POTP_SOURCE})
endif ()

generate_export_header(popt
		BASE_NAME popt
		EXPORT_MACRO_NAME POPT_LIB_EXPORT
		EXPORT_FILE_NAME  ${CMAKE_SOURCE_DIR}/src/poptexport.h
)

target_include_directories(popt PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if (Iconv_FOUND)
	target_link_libraries(popt PRIVATE Iconv::Iconv)
endif()

set_target_properties(popt PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${POPT_SOVERSION}
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS ON
	PUBLIC_HEADER "popt.h;poptexport.h"
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
