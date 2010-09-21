#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([libpillbig], [0.1a], [superruzafa@gmail.com])
AC_CONFIG_SRCDIR([src/pillbig.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Doxygen Configuration
DX_DOXYGEN_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_PDF_FEATURE(OFF)
DX_INIT_DOXYGEN([libpillbig], [Doxyfile], [doc])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 include/Makefile
                 src/Makefile
                 Doxyfile])
AC_OUTPUT
