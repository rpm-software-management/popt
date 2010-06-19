#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([popt],[1.17.DEVEL],[popt-devel@rpm5.org])
AC_CONFIG_SRCDIR([lookup3.c])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET
AM_MAINTAINER_MODE
# Must come before AM_INIT_AUTOMAKE.
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])
AM_ICONV
# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 0)
AC_SUBST(LT_REVISION, 0)
AC_SUBST(LT_AGE, 8)

ALL_LINGUAS="cs da de eo es fi fr ga gl hu id is it ja ko lv nb nl pl pt ro ru sk sl sv th tr uk vi wa zh_TW zh_CN"

CFLAGS="$CFLAGS -D_GNU_SOURCE -D_REENTRANT"

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_SYS_LARGEFILE
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

#  Checks for doxygen.
AC_PATH_PROG(DOXYGEN, doxygen, NO_DOXYGEN)

AC_ARG_ENABLE(doxygen,
    AS_HELP_STRING([--enable-doxygen],[enable doxygen API docs generation]),
    DOXYGEN_ENABLE=$enableval,
   [if test "$DOXYGEN" != NO_DOXYGEN; then
        DOXYGEN_ENABLE=yes
    else
        DOXYGEN_ENABLE=no
    fi]
)

if test "$DOXYGEN_ENABLE" = "yes" && test "$DOXYGEN" = "NO_DOXYGEN"; then
	AC_MSG_WARN([You have asked doxygen docs generation but doxygen isn't in the PATH])
fi
if test "$DOXYGEN_ENABLE" = "no"; then
	DOXYGEN=NO_DOXYGEN
fi
AM_CONDITIONAL([DOXYGEN_ENABLE], test x$DOXYGEN != xNO_DOXYGEN )

# Define for popt_sysconfig 
popt_sysconfdir="${sysconfdir}"
eval "popt_sysconfdir=\"${popt_sysconfdir}\"" # expand contained ${prefix}
AC_DEFINE_UNQUOTED([POPT_SYSCONFDIR], ["$popt_sysconfdir"], [Full path to default POPT configuration directory])

# Check for gcov support.
# Check for lcov  Makefile conditional
HAVE_LCOV=no
AC_ARG_ENABLE(build-gcov,
    AS_HELP_STRING([--enable-build-gcov], [build POPT instrumented for gcov]), [dnl
    if test ".$enableval" = .yes; then
        if test ".`$CC --version 2>&1 | grep 'GCC'`" != .; then
            dnl # GNU GCC (usually "gcc")
            CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
	    AC_CHECK_PROG(HAVE_LCOV,lcov,"yes","no" )
        fi
    fi
])
AM_CONDITIONAL([HAVE_LCOV], test x$LCOV != xno )

# Check for api-sanity-autotest.pl Makefile conditional support for check-local
AC_CHECK_PROG(HAVE_API_SANITY_AUTOTEST,api-sanity-autotest.pl,"yes","no" )

AM_CONDITIONAL([HAVE_API_SANITY_AUTOTEST], test x$HAVE_API_SANITY_AUTOTEST != xno )

# Check for splint Makefile conditional support 
AC_CHECK_PROG(HAVE_SPLINT,splint,"yes","no" )

AM_CONDITIONAL([HAVE_SPLINT], test x$HAVE_SPLINT != xno )

# Check for pmccabe Makefile conditional support 
AC_CHECK_PROG(HAVE_MCCABE,pmccabe,"yes","no" )

AM_CONDITIONAL([HAVE_MCCABE], test x$HAVE_MCCABE != xno )

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([assert.h fcntl.h float.h langinfo.h libintl.h limits.h sys/ioctl.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNC(setreuid, [], [
    AC_CHECK_LIB(ucb, setreuid, [if echo $LIBS | grep -- -lucb >/dev/null ;then :; else LIBS="$LIBS -lc -lucb" USEUCB=y;fi])
])
AC_CHECK_FUNCS(getuid geteuid iconv mtrace __secure_getenv setregid stpcpy strerror vasprintf srandom)
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
AM_CONDITIONAL(HAVE_LD_VERSION_SCRIPT, test "$have_ld_version_script" = "yes")

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
