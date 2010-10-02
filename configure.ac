#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([libpillbig], [0.2.0], [superruzafa@gmail.com])
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

AC_ARG_WITH([test_pillbig],
            [AS_HELP_STRING([--with-test-pillbig],
                            [provides an alternate pill.big file to be used for testing purposes @<:@default=pill.big@:>@])],
            [AC_DEFINE_UNQUOTED([TEST_PILLBIG_FILENAME], ["$withval"], [Defines the pill.big file to be used for testing purposes])],
            [AC_DEFINE([TEST_PILLBIG_FILENAME], ["pill.big"], [Defines the pill.big file to be used for testing purposes])])

# Check for packages
#PKG_CHECK_MODULES([CHECK], [check])
PKG_CHECK_MODULES([XML2], [libxml-2.0])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([check], [suite_create], [CHECK_LIBS=-lcheck; CHECK_LDFLAGS=-pthread])
AC_SUBST([CHECK_LIBS])
AC_SUBST([CHECK_LDFLAGS])

# Checks for header files.
AC_CHECK_HEADERS([check.h],
	[
		AC_DEFINE([HAVE_CHECK], 1, [Define to 1 if you have the <check.h> header file.])
	])

AC_SUBST([CHECK_CFLAGS])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNC([memset])

AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 include/Makefile
                 src/Makefile
                 share/Makefile
                 tests/Makefile
                 Doxyfile])
AC_OUTPUT
