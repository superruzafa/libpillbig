AM_CPPFLAGS = -I$(top_srcdir)/include @CHECK_CFLAGS@

TESTS = file
check_PROGRAMS = file
file_SOURCES = suite.c
file_LDADD = ../lib/libpillbig.la @CHECK_LIBS@
file_LDFLAGS = @CHECK_LDFLAGS@
EXTRA_DIST = file.c
