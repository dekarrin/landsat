AM_CFLAGS=-Wall
AM_CXXFLAGS=${AM_CFLAGS}
LDADD=libstats.la libsort.la libdata.la
AM_LDFLAGS=-static

noinst_LTLIBRARIES=libanalysis.la libstats.la libsort.la libdata.la libparse.la
libdata_la_SOURCES=data.cpp
libstats_la_SOURCES=stats.cpp
libsort_la_SOURCES=sort.cpp
libparse_la_SOURCES=parse.cpp
libanalysis_la_SOURCES=analysis.cpp

bin_PROGRAMS=test landsat
test_SOURCES=test.cpp
landsat_SOURCES=landsat.cpp
landsat_LDADD=libanalysis.la libparse.la ${LDADD} -ltiff
