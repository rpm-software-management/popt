#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([popt],[1.17.DEVEL],[popt-devel@rpm5.org])

AC_CONFIG_SRCDIR([lookup3.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Must come before AM_INIT_AUTOMAKE.
AC_CONFIG_AUX_DIR([build-aux])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
#
# Checks for programs.
# Set common system defines for POSIX extensions, such as GNU_SOURCE
# Must be called before any macros that run the compiler (like AC-PROG-LIBTOOL)
AC_USE_SYSTEM_EXTENSIONS
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])
AM_ICONV
AM_MAINTAINER_MODE
# Libtool library interface versions 
# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 0)
AC_SUBST(LT_REVISION, 0)
AC_SUBST(LT_AGE, 8)



AC_PROG_CC
AC_PROG_CC_STDC
AC_SYS_LARGEFILE
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Turn gcc warning

AC_ARG_ENABLE([gcc-warnings],
  [AS_HELP_STRING([--enable-gcc-warnings],
                  [turn on GCC warnings (for developers)@<:@default=no@:>@])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for gcc-warnings option]) ;;
   esac
   popt_gcc_warnings=$enableval],
  [popt_gcc_warnings=no]
)

AS_IF([test "x$popt_gcc_warnings" = xyes],
  [ # Add/Delete as needed
  popt_CFLAGS_ADD([-Wall], [POPT_CFLAGS])
  popt_CFLAGS_ADD([-pedantic], [POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wextra], [POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat-y2k], [POPT_CFLAGS])
  popt_CFLAGS_ADD([-fdiagnostics-show-option],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-funit-at-a-time],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fstrict-aliasing],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wstrict-overflow],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fstrict-overflow],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wpointer-arith],[POPT_CFLAGS])
  ])


# build popt instrumented for -fPIE (GCC only)
AC_ARG_ENABLE([build-pie],
  [AS_HELP_STRING([--enable-build-pie],
                  [build POPT as Position Independent Executable (PIE) (@<:@default=yes@:>@ with GCC only)])],
  [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for build-pie option]) ;;
   esac
   popt_gcc_pie=$enableval],
  [popt_gcc_pie=yes]
)
AS_IF([test "x$popt_gcc_pie" = xyes],
      [
        AS_IF([test "x$GCC" != x],
              [ popt_CFLAGS_ADD([-fpie], [POPT_CFLAGS])
                popt_LDFLAGS_ADD([-pie], [POPT_LDFLAGS])
              ],
              [ AC_MSG_WARN([--enable-build-pie requires GCC])]
        )
      ])
AC_SUBST([POPT_LDFLAGS])

# build popt instrumented for extra optimization/security (GCC only)
# -fno-delete-null-pointer as the kernel does http://patchwork.kernel.org/patch/36060/
# GNU GCC (usually "gcc")
AS_IF([test "x$GCC" != x],
 [ for c in -fno-delete-null-pointer-checks -D_FORTIFY_SOURCE=2 -fstack-protector -fexceptions
   do
  	popt_CFLAGS_ADD([$c], [POPT_CFLAGS])
   done
 ])
# Misc CFLAGS 
popt_CFLAGS_ADD([-D_REENTRANT], [POPT_CFLAGS])
# Finally put an AC_SUBST for all the CFLAGS above
AC_SUBST([POPT_CFLAGS])

# Support for running test cases using valgrind:

use_valgrind=false
VALGRIND_ENVIRONMENT=""
AC_ARG_ENABLE(valgrind,
[  --enable-valgrind       Use valgrind when running unit tests. ],
[ use_valgrind=true ])

AS_IF([ test x"$use_valgrind" = x"true" ], 
      [
        AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)
        AS_IF([ test x$HAVE_VALGRIND = no ],
              [
                AC_MSG_ERROR([You have asked for Valgrind support but Valgrind not found in PATH. ])
              ],
              [ 
              AS_IF([ test x$enable_shared = xyes ],
                    [ 
              		VALGRIND_ENVIRONMENT="libtool --mode=execute "
                    ]
              )
              VALGRIND_ENVIRONMENT="$VALGRIND_ENVIRONMENT valgrind -q --error-exitcode=3 --leak-check=yes --leak-check=full --log-file=\$(builddir)/popt-valgrind-result --suppressions=\$(srcdir)/popt-vg.supp "
              AC_SUBST([VALGRIND_ENVIRONMENT])
              ])
      ])
     
AM_CONDITIONAL([USE_VALGRIND], [test x$use_valgrind != no])

#  Checks for doxygen support. 
AC_PATH_PROG(DOXYGEN, doxygen, NO_DOXYGEN)
AC_ARG_ENABLE(doxygen,
    AS_HELP_STRING([--disable-doxygen],[disable doxygen API docs generation ]),
    [DOXYGEN_ENABLE=$enableval,
    if test x$DOXYGEN_ENABLE != xno 
    then 
        if test "$DOXYGEN" != NO_DOXYGEN 
        then
           DOXYGEN_ENABLE=yes
        else
           DOXYGEN_ENABLE=no
	   AC_MSG_WARN([You have asked doxygen docs generation but doxygen isn't in the PATH])
        fi
    fi],
    [DOXYGEN_ENABLE=yes])
AM_CONDITIONAL([DOXYGEN_ENABLE], [test x$DOXYGEN != xNO_DOXYGEN])

# Define for popt_sysconfig 
popt_sysconfdir="${sysconfdir}"
eval "popt_sysconfdir=\"${popt_sysconfdir}\"" # expand contained ${prefix}
AC_DEFINE_UNQUOTED([POPT_SYSCONFDIR], ["$popt_sysconfdir"], [Full path to default POPT configuration directory])

# Check for gcov support.
# Check for lcov  Makefile conditional
AC_PATH_PROG(LCOV,lcov, NO_LCOV)
AC_ARG_ENABLE(build-gcov,
    AS_HELP_STRING([--enable-build-gcov], [build POPT instrumented for gcov @<:@default=no@:>@.]), 
    [enable_build_gcov=${enableval}],
    [enable_build_gcov=no])

AS_IF([test "x$enable_build_gcov" != xno],
       [
        if  test "x$GCC" != x
        then
            popt_CFLAGS_ADD([-fprofile-arcs -ftest-coverage], [POPT_CFLAGS])
            AC_SUBST([POPT_CFLAGS])
        else
            LCOV=NO_LCOV
        fi 
       ])
AM_CONDITIONAL([HAVE_LCOV], [test x$LCOV != xNO_LCOV])

# Check for api-sanity-autotest.pl Makefile conditional support for check-local
AC_PATH_PROG(API_SANITY_AUTOTEST,api-sanity-autotest.pl,NO_API_SANITY_AUTOTEST)
AM_CONDITIONAL([HAVE_API_SANITY_AUTOTEST], [test "x$API_SANITY_AUTOTEST" != xNO_API_SANITY_AUTOTEST])
# Check for splint Makefile conditional support 
AC_PATH_PROG(SPLINT,splint,NO_SPLINT)
AM_CONDITIONAL([HAVE_SPLINT],[test "x$SPLINT" != xNO_SPLINT])
# Check for mccabe makefile conditional support
AC_PATH_PROG(MCCABE,pmccabe,NO_MCCABE)
AM_CONDITIONAL([HAVE_MCCABE],[test "x$MCCABE" != xNO_MCCABE])
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([assert.h fcntl.h float.h langinfo.h libintl.h limits.h sys/ioctl.h wchar.h alloca.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_TYPE_INT64_T

# Checks for library functions.
AC_CHECK_FUNC(setreuid, [], [
    AC_CHECK_LIB(ucb, setreuid, [if echo $LIBS | grep -- -lucb >/dev/null ;then :; else LIBS="$LIBS -lc -lucb" USEUCB=y;fi])
])
AC_CHECK_FUNCS([getuid geteuid iconv mtrace __secure_getenv setregid stpcpy strerror vasprintf srandom])
# drop AC_FUNC_MALLOC, REALLOC and STRTOD
AC_CHECK_FUNCS([malloc realloc strtod memset nl_langinfo stpcpy strchr strerror strrchr])

# Checks for ld_version scripts
# For some systems we know that we have ld_version scripts.
# Use it then as default.
have_ld_version_script=no
case "${host}" in
    *-*-linux*)
        have_ld_version_script=yes
        ;;
    *-*-gnu*)
        have_ld_version_script=yes
        ;;
esac
AC_ARG_ENABLE([ld-version-script],
              AS_HELP_STRING([--enable-ld-version-script],[enable/disable use of linker version script.
                              (default is system dependent)]),
              [have_ld_version_script=$enableval],
              [ : ] )
AM_CONDITIONAL([HAVE_LD_VERSION_SCRIPT],[test "$have_ld_version_script" = "yes"])

# Autofu for pkgconfig 
# Define a (hope) portable Libs pkgconfig directive that 
# - Don't harm if the default library search path include ${libdir}
#   (https://bugzilla.novell.com/show_bug.cgi?id=529921)
# - Don't require a not upstream patch to pkgconfig
#   (https://bugs.freedesktop.org/show_bug.cgi?id=16095)
popt_pkgconfig_libs='-L${libdir} -lpopt'
case "${host}" in
    *-*-linux*)
	case "${libdir}" in
	    /usr/lib|/usr/lib64|/lib|/lib64)
       		   popt_pkgconfig_libs='-lpopt'
		;;
    		*)
		popt_pkgconfig_libs='-L${libdir} -lpopt'
	;;
	esac
      ;;
    *-*-gnu*)
	case "${libdir}" in
	    /usr/lib|/usr/lib64|/lib|/lib64)
       		   popt_pkgconfig_libs='-lpopt'
		;;
    		*)
		popt_pkgconfig_libs='-L${libdir} -lpopt'
	;;
	esac
      ;;
esac
AC_SUBST([POPT_PKGCONFIG_LIBS],"$popt_pkgconfig_libs")

AC_CONFIG_FILES([Makefile Doxyfile popt.pc popt.spec po/Makefile.in
    auto/Makefile auto/desc ])
AC_OUTPUT
