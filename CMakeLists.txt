cmake_minimum_required(VERSION 3.12)

# Ensure built-in policies from CMake are used, (e.g. improved policies for macOS)
cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})

project(popt
	VERSION 1.19
	DESCRIPTION "Portable library for parsing command line parameters"
	LANGUAGES C
)

# Set soversion
set(POPT_SOVERSION 0)

# Set up GNU conventions and standard FHS paths
include(GNUInstallDirs)

# Activate CMake package configuration helpers
include(CMakePackageConfigHelpers)

# set the minimum C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

# Check for necessary symbols and headers
include(CheckSymbolExists)
include(CheckIncludeFile)

function(chkdef func inc)
	string(TOUPPER ${func} FUNC)
	set(HAVENAME HAVE_${FUNC})
	check_symbol_exists(${func} "${inc}" ${HAVENAME})
	if (${HAVENAME})
		add_compile_definitions(${HAVENAME})
	endif()
endfunction()

function(chkhdr inc req)
	string(MAKE_C_IDENTIFIER ${inc} ID)
	string(TOUPPER ${ID} INC)
	set(HAVENAME HAVE_${INC})
	check_include_file(${inc} ${HAVENAME})
	if (${HAVENAME})
		add_compile_definitions(${HAVENAME})
	endif()
	if (${req} AND NOT ${HAVENAME})
		message(FATAL_ERROR "required include ${inc} not found")
	endif()
endfunction()

chkdef(stpcpy string.h)
chkdef(strerror string.h)
chkdef(getuid unistd.h)
chkdef(geteuid unistd.h)
chkdef(mtrace mcheck.h)
chkdef(secure_getenv stdlib.h)
chkdef(__secure_getenv stdlib.h)
chkdef(setreuid unistd.h)
chkdef(setuid unistd.h)
chkdef(stpcpy string.h)
chkdef(strerror string)
chkdef(vasprintf stdio)
chkdef(srandom stdlib.h)
chkdef(glob_pattern_p glob.h)
chkdef(mbsrtowcs wchar.h)


set(OPTINCS
	fnmatch.h glob.h langinfo.h libintl.h mcheck.h stdalign.h
)
foreach(inchdr ${OPTINCS})
	chkhdr(${inchdr} FALSE)
endforeach()


add_subdirectory(src)
add_subdirectory(po)
add_subdirectory(doc)

# Enable testing
include(CTest)
enable_testing()
add_subdirectory(tests)
