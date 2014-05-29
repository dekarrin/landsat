#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([landsat-data], [1.0], [dekarrin@outlook.com])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/data.hpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([tiff], [TIFFOpen])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile])
AC_OUTPUT