AM_CPPFLAGS = -I$(top_srcdir)/include $(XML2_CFLAGS)

# Library versioning, fount at http://www.freesoftwaremagazine.com/books/agaal/building_shared_libraries_once_using_autotools

# 1. Start with version information of 0:0:0 for each libtool library. [This is done automatically by simply omitting the -version option from the list of linker flags passed to the libtool script.]
# 2. Update the version information only immediately before a public release of your software. More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# 3. If the library source code has changed at all since the last update, then increment revision (c:r:a becomes c:r+1:a).
# 4. If any interfaces [exported functions or data] have been added, removed, or changed since the last update, increment current, and set revision to 0.
# 5. If any interfaces have been added since the last public release, then increment age.
# 6. If any interfaces have been removed since the last public release, then set age to 0.

CURRENT = 1
AGE = 1
REVISION = 0

lib_LTLIBRARIES = libpillbig.la
libpillbig_la_SOURCES = file.c error.c audio.c adpcm.c vag.h vag.c db.c
libpillbig_la_LDFLAGS = -version-info $(CURRENT):$(REVISION):$(AGE) \
                        $(XML2_LIBS)

EXTRA_DIST = common_internal.h file_internal.h error_internal.h \
             audio_internal.h adpcm.h vag.h

