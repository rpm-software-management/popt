AC_PREREQ(2.57)
AC_INIT(popt, 1.16, rpm-maint@lists.rpm.org)
AC_CONFIG_SRCDIR([popt.h])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_TARGET

dnl Must come before AM_INIT_AUTOMAKE.
dnl AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall])
AM_MAINTAINER_MODE

AC_PROG_CC_STDC
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS

AC_PROG_INSTALL
AC_PROG_LIBTOOL

dnl if CC is gcc, we can rebuild the dependencies (since the depend rule
dnl requires gcc).  If it's not, don't rebuild dependencies -- use what was
dnl shipped with RPM.
if test X"$GCC" = "Xyes"; then
    CFLAGS="$CFLAGS -Wall -W"
    TARGET="depend allprogs"
else
    TARGET="everything"
    dnl let the Makefile know that we're done with `depend', since we don't
    dnl have gcc we're not going to rebuild our dependencies at all.
    echo >.depend-done
fi
AC_SUBST(TARGET)

AC_SYS_LARGEFILE

AC_CHECK_HEADERS(float.h fnmatch.h glob.h langinfo.h libintl.h mcheck.h unistd.h)

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
AC_CHECK_FUNCS(getuid geteuid iconv mtrace __secure_getenv setreuid setuid stpcpy strerror vasprintf srandom)

AM_GNU_GETTEXT_VERSION([0.16.1])
AM_GNU_GETTEXT([external])
AM_ICONV_LINK

popt_sysconfdir="${sysconfdir}"
eval "popt_sysconfdir=\"${popt_sysconfdir}\"" # expand contained ${prefix}
AC_DEFINE_UNQUOTED([POPT_SYSCONFDIR], ["$popt_sysconfdir"], [Full path to default POPT configuration directory])

AC_CONFIG_SUBDIRS()
AC_CONFIG_FILES([ po/Makefile.in
    Doxyfile Makefile popt.pc test-poptrc
    auto/Makefile auto/desc auto/types
])
AC_OUTPUT
