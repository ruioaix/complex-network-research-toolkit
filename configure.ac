#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cnrt], [1.0], [xrfind@gmail.com])
#complex network research toolkit
AC_CONFIG_SRCDIR([toolkit/a.c])

AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h])

AC_CONFIG_FILES([Makefile src/Makefile toolkit/Makefile test/Makefile])
AC_CONFIG_COMMANDS
AC_CONFIG_LINKS

# Checks for programs.
AC_PROG_CC

AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor memset pow strtol])

AC_OUTPUT
