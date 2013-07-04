dnl ##
dnl ##  acinclude.m4 -- manually provided local Autoconf macros
dnl ##

dnl Copyright (C) Elia Pinto (devzero2000@rpm5.org)
dnl Inspired from gnulib warning.m4 and other
dnl but not from ac_archive 


# popt_AS_VAR_APPEND(VAR, VALUE)
# ----------------------------
# Provide the functionality of AS_VAR_APPEND if Autoconf does not have it.
m4_ifdef([AS_VAR_APPEND],
[m4_copy([AS_VAR_APPEND], [popt_AS_VAR_APPEND])],
[m4_define([popt_AS_VAR_APPEND],
[AS_VAR_SET([$1], [AS_VAR_GET([$1])$2])])])

# popt_CFLAGS_ADD(PARAMETER, [VARIABLE = POPT_CFLAGS])
# ------------------------------------------------
# Adds parameter to POPT_CFLAGS if the compiler supports it.  For example,
# popt_CFLAGS_ADD([-Wall],[POPT_CFLAGS]).
AC_DEFUN([popt_CFLAGS_ADD],
[AS_VAR_PUSHDEF([popt_my_cflags], [popt_cv_warn_$1])dnl
AC_CACHE_CHECK([whether compiler handles $1], [popt_my_cflags], [
  save_CFLAGS="$CFLAGS"
  CFLAGS="${CFLAGS} $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
                    [AS_VAR_SET([popt_my_cflags], [yes])],
                    [AS_VAR_SET([popt_my_cflags], [no])])
  CFLAGS="$save_CFLAGS"
])
AS_VAR_PUSHDEF([popt_cflags], m4_if([$2], [], [[POPT_CFLAGS]], [[$2]]))dnl
AS_VAR_IF([popt_my_cflags], [yes], [popt_AS_VAR_APPEND([popt_cflags], [" $1"])])
AS_VAR_POPDEF([popt_cflags])dnl
AS_VAR_POPDEF([popt_my_cflags])dnl
m4_ifval([$2], [AS_LITERAL_IF([$2], [AC_SUBST([$2])], [])])dnl
])

# popt_LDFLAGS_ADD(PARAMETER, [VARIABLE = POPT_LDFLAGS])
# ------------------------------------------------
# Adds parameter to POPT_LDFLAGS if the compiler supports it.  For example,
# popt_LDFLAGS_ADD([-Wall],[POPT_LDFLAGS]).
AC_DEFUN([popt_LDFLAGS_ADD],
[AS_VAR_PUSHDEF([popt_my_ldflags], [popt_cv_warn_$1])dnl
AC_CACHE_CHECK([whether compiler handles $1], [popt_my_ldflags], [
  save_LDFLAGS="$LDFLAGS"
  LDFLAGS="${LDFLAGS} $1"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([])],
                    [AS_VAR_SET([popt_my_ldflags], [yes])],
                    [AS_VAR_SET([popt_my_ldflags], [no])])
  LDFLAGS="$save_LDFLAGS"
])
AS_VAR_PUSHDEF([popt_ldflags], m4_if([$2], [], [[POPT_LDFLAGS]], [[$2]]))dnl
AS_VAR_IF([popt_my_ldflags], [yes], [popt_AS_VAR_APPEND([popt_ldflags], [" $1"])])
AS_VAR_POPDEF([popt_ldflags])dnl
AS_VAR_POPDEF([popt_my_ldflags])dnl
m4_ifval([$2], [AS_LITERAL_IF([$2], [AC_SUBST([$2])], [])])dnl
])

# popt_MSG_STATUS([featurename],[have_feature], [enable_feature])
# --------------------------------------------------------------
AC_DEFUN([popt_MSG_STATUS],
[
   m4_if($#,3,,[m4_fatal([$0: invalid number of arguments: $#])])
   AS_ECHO_N(["    $1: "])
   AS_IF([test "x$3" = "xno"], [AS_ECHO(["$2 (disabled)"])],
         [test "x$3" = "xyes"], [AS_ECHO(["$2"])],
         [test "x$3" = "x"], [AS_ECHO(["$2"])],
         [AS_ECHO(["$2 ($3)"])])
])

