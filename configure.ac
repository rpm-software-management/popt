AC_PREREQ(2.57)
AC_INIT(popt, 1.12, popt-devel@rpm5.org)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([popt.h])
AC_CONFIG_HEADERS([config.h])

# Library code modified:                              REVISION++
# Interfaces changed/added/removed:   CURRENT++       REVISION=0
# Interfaces added:                             AGE++
# Interfaces removed:                           AGE=0
AC_SUBST(LT_CURRENT, 0)
AC_SUBST(LT_REVISION, 0)
AC_SUBST(LT_AGE, 8)

AM_INIT_AUTOMAKE([foreign])

ALL_LINGUAS="cs da de es eu_ES fr gl hu is ja ko no pl pt ro ru sk sl sv tr uk wa"

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

if test "X$CC" = Xgcc; then
    CFLAGS="-Wall $CFLAGS"
fi

AC_GCC_TRADITIONAL
AC_SYS_LARGEFILE

AC_ISC_POSIX
AM_C_PROTOTYPES
AC_CHECK_VA_COPY

dnl XXX lose rpm libs
LIBS=
addlib() {
  l=$1
  shift
  case "$target" in 
    *-*-solaris*)	LIBS="$LIBS -L$l -R $l $*";;
    *)		LIBS="$LIBS -L$l $*";;
  esac
}

dnl
dnl if CC is gcc, we can rebuild the dependencies (since the depend rule
dnl requires gcc).  If it's not, don't rebuild dependencies -- use what was
dnl shipped with RPM.
dnl
if test X"$GCC" = "Xyes" ; then
    TARGET="depend allprogs"
else
    TARGET="everything"
    #
    # let the Makefile know that we're done with `depend', since we don't
    # have gcc we're not going to rebuild our dependencies at all.
    #
    echo > .depend-done
fi
AC_SUBST(TARGET)

dnl XXX Choose /usr/lib or /usr/lib64 for library installs.
MARK64=
if ! echo "${libdir}" | grep -q '64$' ; then
    case "${target_cpu}" in
    x86_64*|powerpc64*|ppc64*|sparc64*|s390x*) MARK64=64 ;;
    esac
fi
AC_SUBST(MARK64)

AC_CHECK_HEADERS(float.h libintl.h mcheck.h unistd.h)

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

if test ! -f ../rpm.c
then
  AC_MSG_CHECKING(for GNU xgettext)
  xgettext --version 2>&1 | grep 'GNU gettext' >/dev/null 2>&1 || AC_MSG_ERROR([
  *** GNU gettext is required. The latest version
  *** is always available from ftp://ftp.gnu.org/gnu/gettext/.])
  AC_MSG_RESULT(yes)
fi

AC_CHECK_FUNC(setreuid, [], [
    AC_CHECK_LIB(ucb, setreuid, [if echo $LIBS | grep -- -lucb >/dev/null ;then :; else LIBS="$LIBS -lc -lucb" USEUCB=y;fi])
])
AC_CHECK_FUNCS(getuid geteuid mtrace __secure_getenv setregid strerror)

AM_GNU_GETTEXT([external])

popt_sysconfdir="${sysconfdir}"
eval "popt_sysconfdir=\"${popt_sysconfdir}\"" # expand contained ${prefix}
AC_DEFINE_UNQUOTED([POPT_SYSCONFDIR], ["$popt_sysconfdir"], [Full path to default POPT configuration directory])

POPT_SOURCE_PATH="`pwd`"
AC_DEFINE_UNQUOTED(POPT_SOURCE_PATH, "$POPT_SOURCE_PATH",
	[Full path to popt top_srcdir.])
AC_SUBST(POPT_SOURCE_PATH)

AC_OUTPUT([Doxyfile Makefile po/Makefile.in])
