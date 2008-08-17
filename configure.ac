#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([QLBar],[0.1.4],[pablo@wizard.ae.krakow.pl])
AM_INIT_AUTOMAKE([-Wall, -Werror foreign])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([Imlib2], [imlib_load_image], , echo -e "Imlib2 is required by qlbar"; exit)
AC_CHECK_LIB([Xext], [XShapeCombineMask], , echo "Xext is required by qlbar"; exit)
AC_CHECK_LIB([pthread], [pthread_create], , echo "pthread is required by qlbar"; exit)

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

#-g is default
#AC_ARG_ENABLE(debug,
#    [  --enable-debug Enable or disable debug symbol generation (default disabled)],
#    [enable_debug="$enableval"],
#    [enable_debug="no"])
#
#AC_MSG_CHECKING([for enable-debug])
#if test x"$enable_debug" = xyes;
#then
# CFGDEBUG=yes
# CXXFLAGS="${CXXFLAGS} -ggdb"
#else
# CFGDEBUG=no
#fi
#AC_MSG_RESULT([$CFGDEBUG])


AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
