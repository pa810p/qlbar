#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([QLBar],[0.2.4],[pablo@wizard.uek.krakow.pl])
AM_INIT_AUTOMAKE([-Wall, -Werror foreign])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([Imlib2], [imlib_load_image], , echo -e "Imlib2 is required by qlbar"; exit)
AC_CHECK_LIB([Xext], [XShapeCombineMask], , echo "Xext is required by qlbar"; exit)

#Pthread is no needed any more
#AC_CHECK_LIB([pthread], [pthread_create], , echo "pthread is required by qlbar"; exit)

# Checks for header files.
AC_PATH_X
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_STAT
AC_CHECK_FUNCS([regcomp])

AC_ARG_WITH([log4cxx], 
    [AS_HELP_STRING([--with-log4cxx], [use log4cxx for logging (default enabled)])],
    [],
    [with_log4cxx=no])

LIBLOG4CXX=
AS_IF([test "x$with_log4cxx" != xno],
    [AC_CHECK_LIB([log4cxx], [main],
        [AC_SUBST([LOG4CXX], ["-llog4cxx"])
        AC_DEFINE([HAVE_LOG4CXX], [1], [Define if you have log4cxx])
        ],
        [AC_MSG_FAILURE(
            [--with-log4cxx was given, but the test for log4cxx failed.])],
        [])])


#-g is default
AC_ARG_ENABLE(debug,
    [  --enable-debug Enable or disable debug symbol generation (default disabled)],
    [enable_debug="$enableval"],
    [enable_debug="no"])

AC_MSG_CHECKING([for enable-debug])
if test x"$enable_debug" = xyes;
then
 CFGDEBUG=yes
 CXXFLAGS="${CXXFLAGS} -ggdb"
else
 CFGDEBUG=no
fi
AC_MSG_RESULT([$CFGDEBUG])


AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
