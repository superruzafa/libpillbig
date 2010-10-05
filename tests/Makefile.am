AM_CPPFLAGS = -I$(top_srcdir)/include @CHECK_CFLAGS@

TESTS = test
check_PROGRAMS = test
test_SOURCES = suite.c file.c db.c
test_LDADD = ../lib/libpillbig.la @CHECK_LIBS@
test_LDFLAGS = @CHECK_LDFLAGS@

