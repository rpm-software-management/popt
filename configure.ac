AC_PREREQ(2.57)
AC_INIT(popt, 1.18, rpm-maint@lists.rpm.org)
AC_CONFIG_SRCDIR([src/popt.h])
AC_CONFIG_HEADERS([config.h])

dnl Must come before AM_INIT_AUTOMAKE.
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 foreign -Wall])

AC_PROG_CC_STDC
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR

AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_SYS_LARGEFILE

AC_CHECK_HEADERS(fnmatch.h glob.h langinfo.h libintl.h mcheck.h stdalign.h)

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
              AC_HELP_STRING([--enable-ld-version-script],
                             [enable/disable use of linker version script.
                              (default is system dependent)]),
              [have_ld_version_script=$enableval],
              [ : ] )
AM_CONDITIONAL(HAVE_LD_VERSION_SCRIPT, test "$have_ld_version_script" = "yes")

AC_ARG_ENABLE(build-gcov,
    AS_HELP_STRING([--enable-build-gcov], [build POPT instrumented for gcov]), [dnl
    if test ".$enableval" = .yes; then
        if test ".`$CC --version 2>&1 | grep 'GCC'`" != .; then
            dnl # GNU GCC (usually "gcc")
            CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
        fi
    fi
])

AC_SEARCH_LIBS(setreuid, [ucb])
AC_CHECK_FUNCS(getuid geteuid iconv mtrace secure_getenv __secure_getenv setreuid setuid stpcpy strerror vasprintf srandom)

AM_GNU_GETTEXT_VERSION([0.18.2])
AM_GNU_GETTEXT([external])
AM_ICONV_LINK

AC_CONFIG_SUBDIRS()
AC_CONFIG_FILES([ po/Makefile.in
    Doxyfile Makefile src/Makefile popt.pc tests/test-poptrc tests/Makefile
])
AC_OUTPUT
