AM_CPPFLAGS = -I$(top_srcdir)/include
BUILT_SOURCES = datadir.h
EXTRA_DIST = params.h
CLEANFILES = datadir.h

bin_PROGRAMS = pillbig
pillbig_SOURCES = pillbig.c params.c
pillbig_LDADD = ../lib/libpillbig.la

datadir.h: Makefile
	echo '#define DATADIR "$(datadir)"' > $@
	
