#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([popt],[1.17.DEVEL],[popt-devel@rpm5.org])

AC_CONFIG_SRCDIR([lookup3.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Must come before AM_INIT_AUTOMAKE.
AC_CONFIG_AUX_DIR([build-aux])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.11 color-tests -Wall -Werror foreign])
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
AM_PROG_AR
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

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
  MAX_STACK_SIZE=32768
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
  popt_CFLAGS_ADD([-Wundef],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat-security],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Winit-self],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmissing-include-dirs],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wunused],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wunknown-pragmas],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wstrict-aliasing],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wshadow],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wbad-function-cast],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wcast-align],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wwrite-strings],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wlogical-op],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Waggregate-return],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wstrict-prototypes],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wold-style-definition],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmissing-prototypes],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmissing-declarations],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmissing-noreturn],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmissing-format-attribute],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wredundant-decls],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wnested-externs],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Winline],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Winvalid-pch],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wvolatile-register-var],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wdisabled-optimization],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wbuiltin-macro-redefined],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmudflap],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wpacked-bitfield-compat],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wsync-nand],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wattributes],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wcoverage-mismatch],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmultichar],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wcpp],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wdeprecated-declarations],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wdiv-by-zero],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wdouble-promotion],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wendif-labels],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat-contains-nul],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat-extra-args],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat-zero-length],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wformat=2],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wmultichar],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wnormalized=nfc],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Woverflow],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wpointer-to-int-cast],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wpragmas],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wsuggest-attribute=const],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wsuggest-attribute=noreturn],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wsuggest-attribute=pure],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wtrampolines],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wno-missing-field-initializers],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wno-sign-compare],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wjump-misses-init],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wno-format-nonliteral],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wframe-larger-than=$MAX_STACK_SIZE],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fstack-protector-all],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fasynchronous-unwind-tables],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fdiagnostics-show-option],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-funit-at-a-time],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-fipa-pure-const],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wno-aggregate-return],[POPT_CFLAGS])
  popt_CFLAGS_ADD([-Wno-redundant-decls],[POPT_CFLAGS])
  ])

# build popt with mudflap instrumentation (GCC only)
popt_gcc_mudflap=no
AC_ARG_ENABLE([mudflap],
 [AS_HELP_STRING([--enable-mudflap],
                  [build POPT with mudflap instrumentation (@<:@default=no@:>@) (with GCC only) (EXPERIMENTAL)])],
 [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for mudflap option]) ;;
   esac
   popt_gcc_mudflap=$enableval],
[ popt_gcc_mudflap=no ])

AS_IF([ test x"$popt_gcc_mudflap" = x"yes" ], 
      [
        AS_IF([test "x$GCC" != x],
              [ 
		old_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -fmudflap"
		old_LDFLAGS="$LDFLAGS"
		LDFLAGS="$LDFLAGS -lmudflap"
		AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[popt_gcc_mudflap=yes],[popt_gcc_mudflap=fail])
		CFLAGS="$old_CFLAGS"
		LDFLAGS="$old_LDFLAGS"
		AS_IF([ test "$popt_gcc_mudflap" = fail ],
                      [
 	  	       AC_MSG_WARN([--enable-mudflap requires a compiler which understands this option.])
                      ],
                      [
                        popt_CFLAGS_ADD([-fmudflap], [POPT_CFLAGS])
                	popt_LDFLAGS_ADD([-lmudflap], [POPT_LDFLAGS])
                        # XXX: memory leak ?
                        #MUDFLAP_OPTIONS="-mode-check -abbreviate -viol-abort -check-initialization -print-leaks"
                        MUDFLAP_OPTIONS="-mode-check -abbreviate -check-initialization"
			AC_SUBST([MUDFLAP_OPTIONS])
                      ])
              ],
              [ AC_MSG_WARN([--enable-mudflap requires GCC])]
        )
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
 [ for c in -fno-delete-null-pointer-checks -Wp,-D_FORTIFY_SOURCE=2 -fstack-protector -fexceptions
   do
  	popt_CFLAGS_ADD([$c], [POPT_CFLAGS])
   done
 ])
# Misc CFLAGS 
popt_CFLAGS_ADD([-D_REENTRANT], [POPT_CFLAGS])
# Finally put an AC_SUBST for all the CFLAGS above
AC_SUBST([POPT_CFLAGS])

# 

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
AC_CHECK_HEADERS([assert.h glob.h fnmatch.h fcntl.h float.h langinfo.h libintl.h limits.h sys/ioctl.h wchar.h alloca.h])

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

dnl check for version script support in the linker (GNU ld, or Solaris ld style)
AC_ARG_ENABLE([ld-version-script],
              AS_HELP_STRING([--enable-ld-version-script],[enable/disable use of linker version script.
                              (default is system dependent)]),
              [have_ld_version_script=$enableval],
              [have_ld_version_script=yes] )

AS_IF([test "x$have_ld_version_script" = xyes],
  [ 
    AC_MSG_CHECKING([if ld version-script works - GNU ld, Solaris ld])
    cat > conftest.map <<EOF
RELEASE
{
	global:
		popt_*;
	local:
		*;
};
PRIVATE
{
	global:
		popti_*;
	local:
		*;
};
EOF
    save_LDFLAGS="$LDFLAGS"
    LDFLAGS="$LDFLAGS -Wl,--version-script=conftest.map"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
                   void popt_symbol1(void) {}
                   void popti_symbol2(void) {}
                   ]], [])],
                   [have_ld_version_script=yes], [have_ld_version_script=no])
    LDFLAGS="$save_LDFLAGS"
    if test x$have_ld_version_script != xyes
    then
    	save_LDFLAGS="$LDFLAGS"
    	LDFLAGS="$LDFLAGS -Wl,-M,conftest.map"
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[
                   void popt_symbol1(void) {}
                   void popti_symbol2(void) {}
                   ]], [])],
                   [have_ld_version_script=yes], [have_ld_version_script=no])
    	LDFLAGS="$save_LDFLAGS"
    	if test x$have_ld_version_script = xyes
    	then
		LD_VERSION_SCRIPT_FLAG=-M
    	fi
    else
      LD_VERSION_SCRIPT_FLAG=--version-script
    fi
    AC_MSG_RESULT($have_ld_version_script)
    rm -f conftest.map conftest.c
])
AC_SUBST([LD_VERSION_SCRIPT_FLAG])
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
